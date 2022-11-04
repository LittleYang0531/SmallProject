<?php
$url = "https://omofun.tv/vod/search.html?wd=" . $argv[1];

function geturl($url){
    $ch = curl_init();
    curl_setopt($ch, CURLOPT_URL, $url);
    curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, FALSE); 
    curl_setopt($ch, CURLOPT_SSL_VERIFYHOST, FALSE); 
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
    $output = curl_exec($ch);
    curl_close($ch);
    return $output;
}

$html = geturl($url);
preg_match_all('/<div[^>]*class="module-card-item-title"[^>]*>(.*?)<\/div>/is', $html, $match);
$title = $match[1];
preg_match_all('/<div[^>]*class="module-info-item-content"[^>]*>(.*?)<\/div>/is', $html, $match);
$tags = $match[1];
$id = array();
for ($i = 0; $i < count($title); $i++) {
    preg_match_all('/<a.*?href="(.*?)".*?>(.*?)<\/a>/is', $title[$i], $match);
    $tmp = explode("/", $match[1][0]);
    $tmp = $tmp[count($tmp) - 1];
    $id[] = substr($tmp, 0, strlen($tmp) - 5);
}
if ($argc == 2) {
    for ($i = 0; $i < count($tags); $i++) $tags[$i] = trim(strip_tags($tags[$i]));
    for ($i = 0; $i < count($title); $i++) {
        echo "Search Result #$i:\n";
        echo "Title: " . strip_tags($title[$i]) . "\n";
        echo "Tags: " . $tags[$i * 2] . "\n";
        echo "Introduce: \n" . $tags[$i * 2 + 1] . "\n\n";
    }
    echo "Use \"php " . $argv[0] . " " . $argv[1] . " [Search ID]\" to download them!\n";
    exit;
}
$aid = intval($argv[2]);
if ($aid >= count($id)) {
    echo "Invalid Search ID!\n";
    exit;
}
$detail_url = "https://omofun.tv/vod/detail/id/" . $id[$aid] . ".html";
$html = geturl($detail_url);
preg_match_all('/<a[^>]*class="module-play-list-link"[^>]*>(.*?)<\/a>/is', $html, $match);
$urls = array();
for ($i = 0; $i < count($match[0]); $i++) {
    preg_match_all('/<a.*?href="(.*?)".*?>(.*?)<\/a>/is', $match[0][$i], $tmp);
    $urls[] = "https://omofun.tv" . $tmp[1][0];
}
$lid = $argc < 4 ? 1 : intval($argv[3]);
$rid = $argc < 5 ? count($urls) : intval($argv[4]);
$psid = $argc < 6 ? -1 : intval($argv[5]);
for ($i = $lid - 1; $i < $rid; $i++) {
    echo "Analysing " . $urls[$i] . "\n";
    $haha = explode("/", $urls[$i]);
    $sid = $haha[count($haha) - 3];
    if ($psid == -1) $psid = intval($sid);
    if (intval($sid) != $psid) {
        echo "Skipped!\n";
        continue;
    }
    $html = geturl($urls[$i]);
    preg_match_all('/<script[^>]*type="text\/javascript"[^>]*>(.*?)<\/script>/is', $html, $match);
    $config = $match[1][3];
    $config = substr($config, 16);
    $json = json_decode($config, true);
    $m3u8_url = $json["url"];
    echo "Downloading $m3u8_url\n";
    system("./m3u8 \"$m3u8_url\" " . ($i + 1) . ".ts 8");
}
?>