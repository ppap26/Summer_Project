<?php
$servername = "localhost";
$username = "root";
$password = "";
$dbname = "sensor_data_db";

$conn = mysqli_connect($servername, $username, $password, $dbname);

// Check connection
if (!$conn) {
    die("Connection failed: " . mysqli_connect_error());
} else {
    echo "database connection successfully";
}

if (isset($_POST["Distance"]) && isset($_POST["Bottle_count"])) {
    $dis = $_POST["Distance"];
    $count = $_POST["Bottle_count"];

    $stmt = $conn->prepare("INSERT INTO bottle_data (distance, bottle_count) VALUES (?, ?)");
    $stmt->bind_param("ii", $dis, $count);

    if ($stmt->execute()) {
        echo "new data created successfully";
    } else {
        echo "Error: " . $stmt->error;
    }

    $stmt->close();
}
$conn->close();
?>