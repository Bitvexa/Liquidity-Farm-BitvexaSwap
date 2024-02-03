#include "farming.hpp"

void farming::init(asset reward, name contract, uint64_t market_id, uint32_t days) {
    require_auth(get_self());

    check(reward.amount > 0, "setel hadiah dengan benar");
    check(days >= 3, "minimal 3 hari");
    check(is_account(contract), "kontrak tidak ditemukan");
    check(token::get_supply(contract, reward.symbol.code()).is_valid(), "token tidak ditemukan");

    Pairs db(BTV_SWAP, BTV_SWAP.value);
    auto &marketData = db.get(market_id, "pasar tidak ditemukan");

    uint64_t hours = days * 24;
    uint64_t seconds = hours * 3600;
    uint64_t now = current_time_point().sec_since_epoch();

    int64_t reward_per_hour = reward.amount / hours;

    Provider provider(BTV_SWAP, marketData.code.raw());
    auto firstProvider = provider.begin();
    check(firstProvider != provider.end(), "belum ada likuiditas");

    State state(get_self(), get_self().value);
    state.emplace(get_self(), [&](auto &row) {
        row.total_reward = reward;
        row.reward_per_hour = {reward_per_hour, reward.symbol};
        row.contract = contract;
        row.next = firstProvider->owner;
        row.duration = days;
        row.start = now;
        row.until = now + seconds;
        row.market_id = market_id;
        row.code = marketData.code;
    });
}

void farming::reset() {
    require_auth(get_self());

    State state(get_self(), get_self().value);
    auto itr = state.begin();
    check(itr != state.end(), "belum dimulai");

    state.erase(itr);
}

void farming::clear() {
    require_auth(get_self());

    uint32_t count = 0, loop = 0, limit = 40;
    Participant users(get_self(), get_self().value);
    for (auto itr = users.begin(); itr != users.end() && loop < limit;) {
        itr = users.erase(itr);
        count++;
        loop++;
    }
    if (count == 0) { print("tidak ada data dihapus"); }
    else { print("hapus ", count, " data"); }
}


void farming::sync() {
    State state(get_self(), get_self().value);
    auto stateData = state.begin();
    check(stateData != state.end(), "belum dimulai");

    uint64_t now = current_time_point().sec_since_epoch();
    check(now <= stateData->until, "sudah berakhir");

    Provider provider(BTV_SWAP, stateData->code.raw());
    auto itrProvider = provider.lower_bound(stateData->next.value);

    for (int i = 0; i < 25 && itrProvider != provider.end(); i++, itrProvider++) {
        if (farming::isExcluded(itrProvider->owner)) continue; // komen baris ini jika tidak perlu
        farming::insertUser(itrProvider->owner, itrProvider->liquidity, now);
    }

    // setel user selanjutnya
    name next;
    string retVal = "oke";
    if (itrProvider != provider.end()) {
        itrProvider++;
        next = itrProvider->owner;
        retVal = "has_more";
    } else {
        next = provider.begin()->owner;
    }
    state.modify(stateData, same_payer, [&](auto &row) {
        row.next = next;
    });
    print(retVal);
}

void farming::insertUser(name user, int64_t liquidity, uint64_t now) {
    Participant users(get_self(), get_self().value);

    auto itr = users.find(user.value);
    if (itr == users.end()) {
        users.emplace(get_self(), [&](auto &row) {
            row.user = user;
            row.liquidity = liquidity;
            row.time = now;
        });
    } else {
        users.modify(itr, same_payer, [&](auto &row) {
            row.liquidity = liquidity;
        });
    }
}

void farming::sendreward() {
    State state(get_self(), get_self().value);
    auto stateData = state.begin();
    check(stateData != state.end(), "belum dimulai");

    uint64_t now = current_time_point().sec_since_epoch();
    if (now > stateData->until) now = stateData->until;
    uint32_t hour = 3600; // 1 jam

    Provider provider(BTV_SWAP, stateData->code.raw());
    Pairs db(BTV_SWAP, BTV_SWAP.value);
    auto &marketData = db.get(stateData->market_id, "pasar tidak ditemukan");

    Participant users(get_self(), get_self().value);
    auto index = users.get_index<"time"_n>();
    auto itr = index.begin();
    int retVal = hour; // delay dalam detik

    for (int i = 0; i < 10; i++) {
        uint64_t distance = now - itr->time;
        uint64_t duration = distance / hour;
        uint64_t timeExcess = distance % hour;
        if (distance < hour) {
            // belum ada 1 jam
            retVal = (hour - distance) + 5;
            break;
        }

        double ratio = (double) itr->liquidity / marketData.liquidity;
        int64_t hourlyReward = stateData->reward_per_hour.amount * ratio;
        asset reward = asset{static_cast<int64_t>(hourlyReward * duration), stateData->total_reward.symbol};

        if (reward.amount > 0) {
            // kirim hadiah
            string memo = "hadiah untuk penyedia likuiditas di Bitvexa Swap";
            action(permission_level{get_self(), "active"_n}, stateData->contract, "transfer"_n,
                   std::make_tuple(get_self(), itr->user, reward, memo)).send();
        } else {
            // hadiah terlalu kecil, loncat ke user berikutnya
            itr++;
            if (itr == index.end()) { itr = index.begin(); }
            else { continue; }
        }

        // periksa apakah user masih pasang likuiditas
        if (provider.find(itr->user.value) != provider.end()) {
            if (stateData->until > now) {
                // perbarui waktu terakhir dapat hadiah
                uint64_t newTime = now;
                if (timeExcess > 0) newTime -= timeExcess;
                index.modify(itr, get_self(), [&](auto &row) {
                    row.time = newTime;
                });
                itr = index.begin();
            } else {
                // farming sudah berakhir
                itr = index.erase(itr);
            }
        } else {
            // user copot likuiditas
            itr = index.erase(itr);
        }
        retVal = 3;
    }
    print(retVal);
}
