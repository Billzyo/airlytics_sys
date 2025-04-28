<?php
$servername = "localhost";
$username = "root";
$password = "";
$dbname = "airlytics";

$conn = new mysqli($servername, $username, $password, $dbname);

$sql = "SELECT * FROM readings ORDER BY timestamp DESC LIMIT 20";
$result = $conn->query($sql);

$data = array();
while ($row = $result->fetch_assoc()) {
  $data[] = $row;
}

echo json_encode(array_reverse($data));
$conn->close();
?>