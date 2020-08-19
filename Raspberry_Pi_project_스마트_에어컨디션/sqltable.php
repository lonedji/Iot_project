<!DOCTYPE html>
<html>
<head>
	<meta charset = "UTF-8">
	<meta http-equiv="refresh" content="30">
	<style type = "text/css">
		.spec{
			text-align:center;
		}
		.con{
			text-align:left;
	}
	</style>
</head>
<body>
	<hi align = "center">My Database</hi>
	<div class = "spec">
		# <b>The sensor value description </b><br>
		# 1 ~ 99 습도 <br>
		# ON OFF 에어컨 상태 
	</div>

	<table border = '1' style = "width = 30%" align = "center">
	<tr align = "center">
		<th>ID</th>
		<th>DATE</th>
		<th>START_TIME</th>
		<th>TEMP</th>
		<th>STATE</th>
		<th>END_TIME</th>
	</tr>

	<?php
		$conn = mysqli_connect("localhost", "emb", "kcci");
		mysqli_select_db($conn, "test");
		$result=mysqli_query($conn, "select * from condi3");

		while($row=mysqli_fetch_array($result)){
		echo "<tr align = center>";
		echo '<td>'.$row['id'].'</td>';
		echo '<td>'.$row['DATE'].'</td>';
		echo '<td>'.$row['START_TIME'].'</td>';
		echo '<td>'.$row['TEMP'].'</td>';
		echo '<td>'.$row['STATE'].'</td>';
		echo '<td>'.$row['END_TIME'].'</td>';

		echo "</tr>";
		mysqli_close($conn);
	

		}
	?>
	</table>
</body>
</html>
