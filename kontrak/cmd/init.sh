# parameter
# jumlah hadiah, token kontrak, market id bitvexa swap, durasi dalam hari


# dalam 9 hari ada hadiah 3000 BTV untuk pair BTV/VEX
cleos -u https://api.databisnis.id push action bitliquidity init '["3000.00000000 BTV", "bitvexatoken", 1, 9]' -p bitliquidity

# dalam 5 hari ada hadiah 200 ribu ZIDR untuk pair BTV/VEX
cleos -u https://api.databisnis.id push action bitliquidity init '["2000000.00 ZIDR", "idr.speakapp", 1, 5]' -p bitliquidity

