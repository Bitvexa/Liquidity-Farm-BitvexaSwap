node=http://209.97.162.124:8080
node2=https://api.databisnis.id

cleos -u $node2 set contract bitliquidity ~/VexaniumProjects/liquidity-farming/kontrak/ farming.wasm farming.abi -p bitliquidity

# copot kontrak
#cleos -u $node2 set contract bitliquidity -c