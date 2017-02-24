# getaddrinfo
When changing /etc/hosts or meddling with dnsmasq, I often wished that there
was a userspace equivalent of getaddrinfo(3) and gethostbyname(3). Behold!

## examples

Get IPv4 TCP address and canonical hostname of mail.google.com:

    $getaddrinfo -f AF_INET -s SOCK_STREAM -l AI_CANONNAME mail.google.com
    AF_INET SOCK_STREAM IPPROTO_TCP 172.217.22.165  googlemail.l.google.com

Simply run gethostbyname:

    $gethostbyname dropbox.com
    hostname: dropbox.com
    address: 108.160.172.238
    address: 108.160.172.206

Simply run gethostbyname2:

    $gethostbyname2 -f AF_INET6 google.com
    hostname: google.com
    address: 2a00:1450:400f:807::200e

Find out which address to listen to (AI\_PASSIVE) 
ssh traffic (port 22) on IPv4 (AF\_INET) TCP (SOCK\_STREAM):

    $getaddrinfo -e 22 -f AF_INET -s SOCK_STREAM -l AI_PASSIVE
    AF_INET SOCK_STREAM IPPROTO_TCP 0.0.0.0



