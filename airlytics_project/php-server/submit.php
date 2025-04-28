<?php
file_put_contents("debug.txt", print_r($_POST, true));

$servername = "localhost";
$username = "root";
$password = "";
$dbname = "airlytics";

$conn = new mysqli($servername, $username, $password, $dbname);
if ($conn->connect_error) {
  die("Connection failed: " . $conn->connect_error);
}

if (isset($_POST['temperature']) && isset($_POST['humidity'])) {
  $temp = $_POST['temperature'];
  $hum = $_POST['humidity'];

  $sql = "INSERT INTO readings (temperature, humidity) VALUES ('$temp', '$hum')";
  if ($conn->query($sql) === TRUE) {
    echo "Data inserted successfully";
  } else {
    echo "Error: " . $conn->error;
  }
} else {
  echo "Missing temperature or humidity!";
}
$conn->close();
?>