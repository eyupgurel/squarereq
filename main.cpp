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



#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
    while(true){
        long epoch_milli_various_order_start = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count();
        int i = 0;
        std::vector<order> v_various_orders;
        prepareOrderVector(250000,1,0.0,11.45, 1242.02,v_various_orders);
        prepareOrderVector(250000, 0,3.02, 3.29,12.01, 1242.02,v_various_orders);
        prepareOrderVector(250000, 0,DBL_MAX, 12.01, 1242.02,v_various_orders);
        prepareOrderVector(250000,1,3.33, 3.48,11.45, 1242.02,v_various_orders);

        nlohmann::json jmsg(v_various_orders);
        zmq::message_t z_out(jmsg.dump());
        sock.send(z_out, zmq::send_flags::none);

        zmq::message_t z_in;
        sock.recv(z_in);
//        std::cout
//                << " thread " << thread_id
//                << "\nsending: " << jmsg.dump()
//                << " received: " << z_in.to_string_view();



        auto jmsg_in = nlohmann::json::parse(z_in.to_string_view());
        for(auto m: jmsg_in){
            match mtch(m["requestingOrderId"],m["respondingOrderId"],m["matchAmount"]);
            i++;
        }

        long epoch_milli_various_order_end = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count();
        auto duration = epoch_milli_various_order_end - epoch_milli_various_order_start;

        cout << "time_elapsed_various_order_processed: " << duration << endl;

        cout << "match size " << i << endl;

        cout << "match per second " << i * 1000 / duration << endl;
    }
#pragma clang diagnostic pop



};

int main() {
    std::thread th0 = std::thread(func, "thread_0");
    //  std::thread th1 = std::thread(func, "thread_1");
    th0.join();
    //th1.join();
}
