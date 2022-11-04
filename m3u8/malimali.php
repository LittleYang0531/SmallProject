<?php
$url = "https://www.malimali6.com/vodsearch/-------------.html?wd=" . $argv[1];

function geturl($url){
    $ch = curl_init();
    curl_setopt($ch, CURLOPT_URL, $url);
    curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, FALSE); 
    curl_setopt($ch, CURLOPT_SSL_VERIFYHOST, FALSE); 
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
    $output = curl_exec($ch);
    if (strpos($output, "系统提示......") !== false) {
        preg_match_all('/<div[^>]*class="text"[^>]*>(.*?)<\/div>/is', $output, $match);
        echo $match[1][0] . "\n";
        echo "--Message from source website malimali6.com!\n";
        exit;
    }
    curl_close($ch);
    return $output;
}

$html = geturl($url);

preg_match_all('/<div[^>]*class="headline clearfix"[^>]*>(.*?)<\/div>/is', $html, $match);
$title = $match[0];
preg_match_all('/<div[^>]*class="des hide"[^>]*>(.*?)<\/div>/is', $html, $match);
$des = $match[0];
preg_match_all('/<div[^>]*class="tags"[^>]*>(.*?)<\/div>/is', $html, $match);
$tags = $match[0];
$id = array();
for ($i = 0; $i < count($title); $i++) {
    preg_match_all('/<a.*?href="(.*?)".*?>(.*?)<\/a>/is', $title[$i], $match);
    $tmp = explode("/", $match[1][0]);
    $tmp = $tmp[count($tmp) - 1];
    $id[] = substr($tmp, 0, strlen($tmp) - 5);
}
if ($argc == 2) {
    for ($i = 0; $i < count($title); $i++) {
        preg_match_all('/<a[^>]*class="title"[^>]*>(.*?)<\/a>/is', $title[$i], $match);
        $tags[$i] = str_replace("</i>", "</i> | ", $tags[$i]);
        $tags[$i] = strip_tags(html_entity_decode($tags[$i]));
        $tags[$i] = substr($tags[$i], 3);
        echo "Search Result #$i:\n";
        echo "Title: " . $match[1][0] . "\n";
        echo "Tags: " . $tags[$i] . "\n";
        echo "Introduce: \n" . strip_tags($des[$i]) . "\n\n";
    }
    echo "Use \"php " . $argv[0] . " " . $argv[1] . " [Search ID]\" to download them!\n";
    exit;
}
$aid = intval($argv[2]);
if ($aid >= count($id)) {
    echo "Invalid Search ID!\n";
    exit;
}
$detail_url = "https://www.malimali6.com/voddetail/" . $id[$aid] . ".html";
$html = geturl($detail_url);
preg_match_all('/<a[^>]*title=\'第[\d]*集\'[^>]*>(.*?)<\/a>/is', $html, $match);
$urls = array();
for ($i = 0; $i < count($match[0]); $i++) {
    if (strlen($match[0][$i]) > 200) continue;
    preg_match_all('/<a.*?href=\'(.*?)\'.*?>(.*?)<\/a>/is', $match[0][$i], $tmp);
    $urls[] = "https://www.malimali6.com" . $tmp[1][0];
}
$lid = $argc < 4 ? 1 : intval($argv[3]);
$rid = $argc < 5 ? count($urls) : intval($argv[4]);
$psid = $argc < 6 ? -1 : intval($argv[5]);
for ($i = $lid - 1; $i < $rid; $i++) {
    echo "Analysing " . $urls[$i] . "\n";
    $haha = explode("/", $urls[$i]);
    $sid = $urls[$i][strlen($urls[$i]) - 8];
    if ($psid == -1) $psid = intval($sid);
    if (intval($sid) != $psid) {
        echo "Skipped!\n";
        continue;
    }
    $html = geturl($urls[$i]);
    preg_match_all('/<script type="text\/javascript">(.*?)<\/script>/is', $html, $match);
    $config = $match[1][1];
    $config = substr($config, 16);
    $json = json_decode($config, true);
    $m3u8_url = $json["url"];
    echo "Downloading $m3u8_url\n";
    system("./m3u8 \"$m3u8_url\" " . ($i + 1) . ".ts 8");
}
?>