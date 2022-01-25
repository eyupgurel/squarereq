#include <iostream>
#include <thread>
#include <chrono>
#include <random>
#include <zmq_addon.hpp>
#include <nlohmann/json.hpp>
#include <data_stream.h>
#include <matching_engine.h>

using namespace std::chrono_literals;

static zmq::context_t ctx;
//static zmq::context_t ctx_pub;


void to_json(nlohmann::json& j, const order& o) {
    j = nlohmann::json{
            {"price", o.price},
            {"epochMilli", o.epochMilli},
            {"quantity", o.quantity},
            {"id", o.id},
            {"ot", o.ot},
            {"cud", o.cud},
    };
}

void from_json(const nlohmann::json& j, order& o) {
    o.price = j.at("price").get<double>();
    o.epochMilli = j.at("epochMilli").get<long>();
    o.quantity = j.at("quantity").get<double>();
    o.id = j.at("id").get<long>();
    o.ot = j.at("ot").get<int>();
    o.cud = j.at("cud").get<int>();
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

void to_json(nlohmann::json& j, const engine_state& es) {
    j = nlohmann::json{
            {"asks", es.asks},
            {"bids", es.bids},
            {"matches", es.matches}
    };
}

void from_json(const nlohmann::json& j, engine_state& es) {
    auto asks = j.at("asks").get<nlohmann::json::array_t>();
    for(auto ask : asks){
        order ord(ask["price"],ask["epochMilli"],ask["quantity"],ask["id"],ask["ot"],ask["cud"]);
        es.asks.emplace_back(ord);
    }
    auto bids = j.at("bids").get<nlohmann::json::array_t>();
    for(auto bid : bids){
        order ord(bid["price"],bid["epochMilli"],bid["quantity"],bid["id"],bid["ot"],bid["cud"]);
        es.bids.emplace_back(ord);
    }
    auto matches = j.at("matches").get<nlohmann::json::array_t>();
    for(auto j_match : matches){
        match match(j_match["requestingOrderId"],j_match["respondingOrderId"],j_match["matchAmount"]);
        es.matches.emplace_back(match);
    }
}


auto func = [](const std::string& url,  const std::string& thread_id){
    zmq::socket_t sock(ctx, zmq::socket_type::req);
    sock.connect(url);


#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
    while(true){
        long epoch_milli_various_order_start = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count();
        int i = 0;
        std::vector<order> v_various_orders;

        prepareOrderVector(5000, 0,3.02, 3.29,12.01, 1242.02,v_various_orders);
        prepareOrderVector(5000,1,3.33, 3.48,11.45, 1242.02,v_various_orders);


        TOrders bids;
        for(int i = 0; i<5; i++){
            bids.insert(v_various_orders[i]);
        }

        print_orders(bids);

        TOrders asks;
        for(int i = 5; i<10; i++){
            asks.insert(v_various_orders[i]);
        }

        print_orders(asks);



        prepareOrderVector(5000,1,0.0,11.45, 1242.02,v_various_orders);
        prepareOrderVector(5000, 0,DBL_MAX, 12.01, 1242.02,v_various_orders);


        TOrders active_sell;
        active_sell.insert(v_various_orders[10]);
        print_orders(active_sell);





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

        engine_state es;
        from_json(jmsg_in,es);

        print_engine_state(es);

/*        for(auto m: jmsg_in){
            match mtch(m["requestingOrderId"],m["respondingOrderId"],m["matchAmount"]);
            i++;
        }*/

        long epoch_milli_various_order_end = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count();
        auto duration = epoch_milli_various_order_end - epoch_milli_various_order_start;

        cout << "time_elapsed_various_order_processed: " << duration << endl;

        cout << "match size " << i << endl;

        cout << "match per second " << i * 1000 / duration << endl;
    }
#pragma clang diagnostic pop



};

auto func_sub = [](const std::string& url,  const std::string& thread_id){
    zmq::socket_t sock(ctx, zmq::socket_type::sub);
    sock.connect(url);
    while(true){
        zmq::message_t z_in;
        sock.recv(z_in, zmq::recv_flags::none);
        std::cout
                << " received: " << z_in.to_string_view() << endl;

    }
};

int main(int argc, char * argv[]) {
    std::thread th0 = std::thread(func, argv[1], "thread_0");
    //std::thread th1 = std::thread(func_sub, argv[2], "thread_1");
    th0.join();
    //th1.join();
}
