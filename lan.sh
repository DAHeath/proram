brew services restart shopify/shopify/toxiproxy
toxiproxy-cli create SLOW --listen localhost:44444 -u localhost:44445
toxiproxy-cli toxic add SLOW -t latency -a latency=2
toxiproxy-cli toxic add SLOW -t bandwidth -a rate=125000
