<?php
	if(!empty($_GET))
	{
		echo "<p>val_a : " . $_GET['val_a'] . "</p>";
		echo "<p>val_b : " . $_GET['val_b'] . "</p>";
	}
	else
		echo "<p>nothing received</p>";

	print_r($_GET);

?>
