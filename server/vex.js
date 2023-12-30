const fetch = require('node-fetch');
const {Api, JsonRpc} = require('vexaniumjs');
const {TextEncoder, TextDecoder} = require('util');
const {JsSignatureProvider} = require("vexaniumjs/dist/vexjs-jssig");

const nodes = {
    'databisnis': 'https://api.databisnis.id',
    'proit': 'https://api.vex.proit.id',
    'speakapp': 'https://vex.speakapp.me',
    'vexascan': 'https://v2.vexascan.com:2096'
};

function buildSignatureProvider() {
    return new JsSignatureProvider([process.env.CRON_KEY]);
}

function initVexjs(node) {
    let signatureProvider = buildSignatureProvider();
    let rpc = new JsonRpc(nodes[node], {fetch});
    let api = new Api({rpc, signatureProvider, textEncoder: new TextEncoder(), textDecoder: new TextDecoder()});
    return {rpc, api};
}

module.exports = {initVexjs};