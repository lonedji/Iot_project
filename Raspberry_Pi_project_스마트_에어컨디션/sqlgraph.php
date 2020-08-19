
<?php
$conn = mysqli_connect("localhost", "emb", "kcci");
mysqli_set_charset($conn, "utf8");
mysqli_select_db($conn, "test");
$result=mysqli_query($conn, "SELECT DATE, END_TIME-START_TIME as time_diff FROM condi3");
$data = array(array('날짜','사용시간'));
if($result)
{
        while($row = mysqli_fetch_array($result))
        {
                array_push($data, array($row[0],intval($row[1])));
        }
}
$options = array(
        'title' => '사용시간  (단위: 초)',
        'width' => 1000, 'height' => 500
);
?>
<script src="//www.google.com/jsapi"></script>
<script>
var data = <?= json_encode($data) ?>;
var options = <?= json_encode($options) ?>;
google.load('visualization', '1.0', {'packages':['corechart']});
google.setOnLoadCallback(function() {
  var chart = new google.visualization.ColumnChart(document.querySelector('#chart_div'));
  chart.draw(google.visualization.arrayToDataTable(data), options);
});
</script>
<div id="chart_div"></div>

