<?php
$servername = "localhost";
$username = "root";
$password = "";
$dbname = "airlytics";

$conn = new mysqli($servername, $username, $password, $dbname);
$sql = "SELECT temperature, humidity, timestamp FROM readings ORDER BY id DESC LIMIT 1";
$result = $conn->query($sql);

if ($result->num_rows > 0) {
  $row = $result->fetch_assoc();
  echo json_encode([
    'temperature' => floatval($row['temperature']),
    'humidity' => floatval($row['humidity']),
    'timestamp' => $row['timestamp']
  ]);
} else {
  echo json_encode(['temperature' => null, 'humidity' => null, 'timestamp' => null]);
}
$conn->close();
?>