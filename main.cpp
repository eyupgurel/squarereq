#include <iostream>
#include <thread>
#include <chrono>
#include <random>
#include <zmq_addon.hpp>
#include <nlohmann/json.hpp>
#include <data_stream.h>

using namespace std::chrono_literals;

static zmq::context_t ctx;


void to_json(nlohmann::json& j, const order& o) {
    j = nlohmann::json{
            {"price", o.price},
            {"epochMilli", o.epochMilli},
            {"quantity", o.quantity},
            {"id", o.id},
            {"ot", o.ot}
    };
}

void from_json(const nlohmann::json& j, order& o) {
    o.price = j.at("price").get<double>();
    o.epochMilli = j.at("epochMilli").get<long>();
    o.quantity = j.at("quantity").get<double>();
    o.id = j.at("id").get<long>();
    o.ot = j.at("ot").get<int>();
}


void to_json(nlohmann::json& j, const match& m) {
    j = nlohmann::json{
            {"requestingOrderId", m.requestingOrderId},
            {"respondingOrderId", m.respondingOrderId},
            {"matchAmount", m.matchAmount}
    };
}

void from_json(const nlohmann::json& j, match& m) {
    m.requestingOrderId = j.at("requestingOrderId").get<long>();
    m.respondingOrderId = j.at("respondingOrderId").get<long>();
    m.matchAmount = j.at("matchAmount").get<double>();
}

auto func = [](const std::string& thread_id){
    zmq::socket_t sock(ctx, zmq::socket_type::req);
    sock.connect("tcp://127.0.0.1:5555");

    int i = 0;

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
    while(true){

        cout << i << endl;


        std::vector<order> v_asks;
        prepareOrderVector(10000,1,3.33, 3.48,11.45, 1242.02,v_asks);
        nlohmann::json jmsg(v_asks);
        zmq::message_t z_out(jmsg.dump());
        sock.send(z_out, zmq::send_flags::none);

        zmq::message_t z_in;
        sock.recv(z_in);
/*        std::cout
                << " thread " << thread_id
                << "\nsending: " << jmsg.dump()
                << " received: " << z_in.to_string_view();*/

        i+= 10000;


/*        int i = 0;
        for(order o: v_asks){
            //order o = v_asks[0];
            i++;
            cout << i << endl;
            nlohmann::json jmsg;
            jmsg["price"] = o.price;
            jmsg["epochMilli"] = o.epochMilli;
            jmsg["quantity"] = o.quantity;
            jmsg["id"] = o.id;
            jmsg["ot"] = 0; // o.ot;
            zmq::message_t z_out(jmsg.dump());
            sock.send(z_out, zmq::send_flags::none);

            zmq::message_t z_in;
            sock.recv(z_in);
*//*            std::cout
                    << " thread " << thread_id
                    << "\nsending: " << jmsg.dump()
                    << " received: " << z_in.to_string_view();*//*
        }*/



        //std::this_thread::sleep_for(500ms);


    }
#pragma clang diagnostic pop



};

int main() {
    std::thread th0 = std::thread(func, "thread_0");
    //  std::thread th1 = std::thread(func, "thread_1");
    th0.join();
    //th1.join();
}
