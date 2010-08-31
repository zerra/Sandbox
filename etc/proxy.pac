// proxy configuration

function FindProxyForURL(url, host) {
  if
  (
    !(
      // go direct for these hosts
      shExpMatch(host, "localhost") ||
      shExpMatch(host, "127.0.0.1")
    )
    &&
    (
      // proxy for radiko
      shExpMatch(host, "radiko.jp") ||
      shExpMatch(host, "log.radiko.jp") ||
      shExpMatch(host, "www.radiko.jp") ||
      shExpMatch(host, "radiko.smartstream.ne.jp") ||
      shExpMatch(host, "radiko-dl.ssdl1.smartstream.ne.jp")
    )
    &&
    !(
      // Current version of squid does not do IPv6.
      shExpMatch(host, "ipv6.*") ||
      /^\[.*:.*\]/.test(host)
    )
  )
    return "PROXY localhost:3128; DIRECT";
  else
    return "DIRECT";
}
