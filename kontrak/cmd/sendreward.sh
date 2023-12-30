# panggil pakai cleos
cleos -u https://api.databisnis.id push action bitliquidity sendreward '[]' -p bitliquidity


# panggil pakai cron
# ganti kontraknya
curl -X POST -H 'Content-Type: application/json' -i 'http://cron.bitvexa.id/api/liquidity-farming/sendreward' --data '{
  "contract": "bitliquidity"
}'