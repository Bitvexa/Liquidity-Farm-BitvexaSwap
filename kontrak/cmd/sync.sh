# panggil pakai cleos
cleos -u https://api.databisnis.id push action bitliquidity sync '[]' -p bitliquidity


# panggil pakai cron
# ganti kontraknya
curl -X POST -H 'Content-Type: application/json' -i 'http://cron.bitvexa.id/api/liquidity-farming/sync' --data '{
  "contract": "bitliquidity"
}'