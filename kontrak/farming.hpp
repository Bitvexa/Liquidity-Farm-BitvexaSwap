#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/system.hpp>
#include <eosio.token/eosio.token.hpp>

using namespace eosio;
using namespace std;

static constexpr name BTV_SWAP = name("bitvexaswap");

CONTRACT farming : public contract {
public:
    using contract::contract;

    TABLE state {
        asset total_reward;
        asset reward_per_hour;
        name contract;
        name next;
        uint32_t duration;
        uint64_t start;
        uint64_t until;
        uint64_t market_id;
        symbol_code code;

        uint64_t primary_key() const { return 1; }
    };
    using State = eosio::multi_index<"state"_n, state>;

    TABLE participant {
        name user;
        uint64_t liquidity;
        uint64_t time; // waktu mulai/terakhir beri hadiah
        uint64_t primary_key() const { return user.value; }
        uint64_t by_time() const { return time; }
    };
    using Participant = eosio::multi_index<"participant"_n, participant,
            indexed_by<"time"_n, const_mem_fun<participant, uint64_t, &participant::by_time>> >;


    ACTION init(asset reward, name contract, uint64_t market_id, uint32_t days);
    ACTION reset();
    ACTION clear();
    ACTION sync();
    ACTION sendreward();

    void insertUser(name user, int64_t liquidity, uint64_t now);

private:
    struct pairs {
        uint64_t            id;
        symbol_code         code;
        name                status;
        extended_asset      reserve0;
        extended_asset      reserve1;
        int64_t             liquidity;
        uint64_t            amplifier;
        uint8_t             fee;
        double              price0_last;
        double              price1_last;
        asset               volume0;
        asset               volume1;
        uint64_t            swap;
        uint32_t            updated;

        uint64_t primary_key() const { return id; }
    };
    using Pairs = eosio::multi_index<"pairs"_n, pairs>;

    struct provider {
        name        owner;
        uint64_t    liquidity;
        asset       reserve0;
        asset       reserve1;
        uint32_t    locked_until;
        uint32_t    created;
        uint32_t    updated;

        uint64_t primary_key() const { return owner.value; }

    };
    using Provider = eosio::multi_index<"provider"_n, provider>;

};
