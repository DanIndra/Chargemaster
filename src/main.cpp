#include <memory>
#include <string>
#include <utility>

#include "mbed.h"
#include "EthernetInterface.h"
#include "http_request.h"

#include "log.hpp"
#include "charger.hpp"
#include "display.hpp"
#include "smart_command.hpp"

EthernetInterface net;

const PinName PWM_IN_PIN = D8;
const PinName PWM_OUT_PIN = D10;
int   relay1 = D12;
int   relay2 = D13;

#ifdef OVO_HOSTNAME
const std::string hostname("ovo-smart-charger");
#else
const std::string hostname("indra-smart-charger");
#endif

const int PORT = 2000;

typedef struct {
    std::shared_ptr<TCPSocket> sock;
    SocketAddress address;
} AppConnection;

typedef std::pair<nsapi_error_t,SmartCommand> ReceivedCommand;

void dump_http_response(HttpResponse* res) {
    log("http status: %d - %s\r\n", res->get_status_code(), res->get_status_message().c_str());

    log("headers:\r\n");
    for (size_t ix = 0; ix < res->get_headers_length(); ix++) {
        log("\t%s: %s\r\n", res->get_headers_fields()[ix]->c_str(), res->get_headers_values()[ix]->c_str());
    }
    log("body (%d bytes):\r\n%s\r\n", res->get_body_length(), res->get_body_as_string().c_str());
}

nsapi_error_t register_dynamic_dns(EthernetInterface *network) {
    std::string ip(network->get_ip_address());
    std::string url;
    url.append("http://www.duckdns.org/update?domains=");
    url.append(hostname);
    url.append("&token=1b242417-7686-489d-aae8-05815376fe61&ip=");
    url.append(ip);

    std::shared_ptr<HttpRequest> get_req(new HttpRequest(network, HTTP_GET, url.c_str()));

    HttpResponse* get_res = get_req->send();

    if (!get_res) {
        return get_req->get_error();
    }

    dump_http_response(get_res);

    log("registered dynamic dns name '%s'\r\n", hostname.c_str());
    return 0;
}

ReceivedCommand recv_command(TCPSocket* sock) {
    SmartCommand err_cmd = SmartCommand(SmartCommand::ERROR);

    char buf[2];
    char* cur = buf;
    nsapi_size_t remain = 2;

    while(remain > 0) {
        nsapi_size_or_error_t n = sock->recv(cur, remain);
        if(n < 0) {
            log("ERR: could not read from socket: %d\r\n", n);
            return std::make_pair(n, err_cmd);
        } else if(n == 0) {
            log("ERR: disconnected\r\n");
            return std::make_pair(NSAPI_ERROR_NO_CONNECTION, err_cmd);
        } else {
            remain -= n;
            cur += n;
        }
    }

    log("buffer 0: %d\r\n", buf[0]);
    log("buffer 1: %d\r\n", buf[1]);


    return std::make_pair(NSAPI_ERROR_OK, SmartCommand::from_buffer(buf));
}

void handle_command(Charger& charger, SmartCommand& cmd) {
    switch(cmd.id()) {
	case SmartCommand::V2G_SWITCH_char0:
		charger.charge0();
		break;
	case SmartCommand::V2G_SWITCH_char6:
		charger.charge6();
		break;
	case SmartCommand::V2G_SWITCH_char10:
		charger.charge10();
		break;
	case SmartCommand::V2G_SWITCH_char16:
		charger.charge16();
		break;
	case SmartCommand::V2G_SWITCH_char24:
		charger.charge24();
		break;
    case SmartCommand::V2G_SWITCH_IDLE:
        charger.idle();
		break;
    case SmartCommand::V2G_SWITCH_CHARGE:
        charger.charge();
		break;
    case SmartCommand::MAX_CHARGE:
        charger.amps(cmd.max_amps());
        break;
    default: log("UNHANDLED COMMAND: %d\r\n", cmd.to_string());
    }
}

void handle_connection(AppConnection conn, Charger& charger) {
    log("connection handler start\r\n");

    log("connectde to: %s\r\n", conn.address.get_ip_address());

    bool running = true;

    while(running) {
        ReceivedCommand cmd = recv_command(conn.sock.get());

        if(cmd.first < 0) {
            log("ERR: bad command\r\n");
            running = false;
        }

        log("COMMAND: %s\r\n", cmd.second.to_string().c_str());
        log("AMPS: %f\r\n", cmd.second.max_amps());

        handle_command(charger, cmd.second);
    }

    conn.sock->close();

    log("connection handler finish\r\n");
}

int main(int argc, char** argv) {
    log("main\r\n");

    Display d;

    // start flashing 'startup' led sequence
    log("starting display\r\n");
    d.pattern(Display::STARTUP);
    d.start();

    // create charger and discover state
    log("creating charger\r\n");
    Charger charger(d, PWM_IN_PIN, PWM_OUT_PIN);

    log("charger state: %d\r\n", charger.state());

    // initialise IP stack
    log("connecting to network\r\n");
    net.connect();

    const char *ip = net.get_ip_address();

    if(ip) {
        log("local ip address: %s\r\n", ip);
    } else {
        log("ERR: no ip address\r\n");
        d.pattern(Display::ERROR);
        return -1;
    }

    nsapi_error_t rc = NSAPI_ERROR_OK;

    rc = register_dynamic_dns(&net);

    if(rc != NSAPI_ERROR_OK) {
        log("ERR: cannot register dynamic dns address - code: %d\r\n", rc);
        d.pattern(Display::ERROR);
        return -1;
    }

    log("creating server socket\r\n");

    TCPServer server(&net);

    rc = server.bind(PORT);

    if(rc != NSAPI_ERROR_OK) {
        log("ERR: cannot bind to port %d\r\n", PORT);
        d.pattern(Display::ERROR);
        return -1;
    }

    rc = server.listen(5);

    if(rc != NSAPI_ERROR_OK) {
        log("ERR: cannot listen on server socket\r\n");
        d.pattern(Display::ERROR);
        return -1;
    }

    bool running = true;

    while(running) {
        AppConnection conn;
        conn.sock.reset(new TCPSocket());

        log("waiting to accepting connections...\r\n");
        rc = server.accept(conn.sock.get(), &conn.address);

        if(rc != NSAPI_ERROR_OK) {
            log("ERR: failed to accept \r\n");
        } else {
            handle_connection(conn, charger);
        }
    }

    log("shutting down\r\n");

    // flash shutdown sequence
    d.pattern(Display::SHUTDOWN);
    Thread::wait(3000);

    // shutdown display
    d.stop();
    log("display stopped\r\n");

    // shutdown IP stack
    net.disconnect();
    log("network disconnected\r\n");

    log("main end\r\n");
    return 0;
}
