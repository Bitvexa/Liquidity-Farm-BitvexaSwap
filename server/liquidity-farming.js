const {initVexjs} = require('./vex');
const Http = require('http-status-codes');

const {api, rpc} = initVexjs("speakapp");
const task = {sync: {}, send: {}};
const account = "cronrunner"; // ganti dengan akun yang panggil kontrak

async function syncLiquidity(contract) {
    await api.getAbi(contract);
    const tx = api.buildTransaction();
    tx.with(contract).as(account).sync();
    let res = await tx.send({blocksBehind: 3, expireSeconds: 30});
    let retVal = res.processed.action_traces[0].console;
    if (retVal.length > 0) {
        let delay = retVal.startsWith("has_more") ? 2000 : 900000; // 2 detik atau 15 menit
        let taskID = task.sync[contract];
        if (taskID > 0) clearTimeout(taskID);
        task.sync[contract] = setTimeout(syncLiquidity, delay, contract);
    }
}

async function doSendReward(contract) {
    await api.getAbi(contract);
    const tx = api.buildTransaction();
    tx.with(contract).as(account).sendreward();
    let res = await tx.send({blocksBehind: 3, expireSeconds: 30});
    let retVal = res.processed.action_traces[0].console;
    if (retVal.length > 0) {
        let delay = parseInt(retVal) * 1000;
        let taskID = task.send[contract];
        if (taskID > 0) clearTimeout(taskID);
        task.send[contract] = setTimeout(doSendReward, delay, contract);
    }
    return task.send[contract];
}


async function sync(req, res) {
    const contract = req.body.contract || '';
    if (contract.length === 0) {
        return res.status(Http.BAD_REQUEST).json("alamat kontrak kosong");
    }
    if (!task.sync.hasOwnProperty(contract)) {
        task.sync[contract] = 0;
    }

    try {
        await syncLiquidity(contract);
        return res.json("berhasil");
    } catch (e) {
        return res.json(e.message);
    }
}

async function sendReward(req, res) {
    const contract = req.body.contract || '';
    if (contract.length === 0) {
        return res.status(Http.BAD_REQUEST).json("alamat kontrak kosong");
    }
    if (!task.send.hasOwnProperty(contract)) {
        task.send[contract] = 0;
    }

    try {
        let id = await doSendReward(contract);
        return res.json(`task id: ${id}`);
    } catch (e) {
        return res.json(e.message);
    }
}

module.exports = {sync, sendReward};
