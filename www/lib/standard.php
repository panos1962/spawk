<?php

Globals::setup();

class Globals {
	private static $server = NULL;

	public static function setup() {
		switch ($_SERVER["HTTP_HOST"]) {
		case "127.0.0.1":
		case "localhost":
			$url = "://localhost/spawk.info/";
			break;
		case "opasopa.net":
		case "www.opasopa.net":
			$url = "://opasopa.net/spawk/";
			break;
		case "www.spawk.info":
		case "spawk.info":
			$url = "://spawk.info/";
			break;
		case "spawk.opasopa.net":
			$url = "://spawk.opasopa.net/";
			break;
		default:
			print $_SERVER["HTTP_HOST"] . ": unknown server";
			die(2);
		}

		self::$server = ((isset($_SERVER['SERVER_PORT']) &&
			($_SERVER['SERVER_PORT'] == 443)) ?
			"https" : "http") . $url;
	}

	public static function url($x) {
		return self::$server . $x;
	}

	public static function head() {
		?>
		<title>SPAWK</title>
		<link rel="icon" type="image/png" href="<?php
			print self::url("image/icon/spawk70x70.png"); ?>">
		<link rel="stylesheet" href="<?php print self::url("lib/standard.css"); ?>">
		<script src="https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js"></script>
		<script src="<?php print self::url("lib/standard.js"); ?>"></script>
		<?php
	}

	public static function top($self = NULL) {
		?>
		<div id="top">
		<?php Globals::tab("SPAWK", self::url("index.html")); ?>
		<?php $self === "home" || Globals::tab("Home", self::url("spawk.php")); ?>
		<?php $self === "tutorial" || Globals::tab("Tutorial", self::url("tutorial")); ?>
		<?php $self === "reference" || Globals::tab("Reference", self::url("reference")); ?>
		<?php $self === "download" || Globals::tab("Download", self::url("download")); ?>
		<?php $self === "about" || Globals::tab("About", self::url("about")); ?>
		</div>
		<?php
	}

	public static function bottom($date = NULL) {
		?>
		<div id="bottomContainer">
		<div id="bottom">
		<table style="width: 100%;">
		<tr>
		<td style="width: 40%;">
		<?php
		/*
		<div id="donate" title="Buy me a beer!">
		<form action="https://www.paypal.com/cgi-bin/webscr" method="post" target="_blank">
		<input type="hidden" name="cmd" value="_s-xclick">
		<input type="hidden" name="hosted_button_id" value="ZJLAZ8BTKD9VA">
		<div class="tab" style="position: relative;">
		<a href="#" target="_self">Donate</a>
		<input id="donateButton" type="image" src="<?php print self::url("image/misc/donate.png");
			?>" border="0" name="submit" alt="PayPal - The safer, easier way to pay online!" />
		</div>
		</form>
		</div>
		*/
		?>
		</td>
		<td style="width: 10%; white-space: nowrap; text-align: center;"
		<?php
		if ($date !== NULL) {
			?> title="Last modified at <?php print $date; ?>"><?php
			print $date;
		}
		else {
			?>&lt;<?php
		}
		?>
		</td>
		<td style="width: 50%; text-align: right;">
		<div id="copyright">Copyright &copy; 2004-<?php print date("Y");
			?>, by Panos Papadopoulos.</div>
		</td>
		</div>
		</div>
		<?php
	}

	public static function tab($title, $href, $target = "_self") {
		?><div class="tab"><a href="<?php print $href; ?>" target="_self"><?php
			print $title; ?></a></div><?php
	}

	public static function toc_item($tag, $title, $level, $class = NULL) {
		?>
		<a id="TOC_<?php print $tag; ?>" name="<?php print $tag; ?>"></a><h<?php
			print $level; ?> title="Table of contents" class="tocItem<?php
				if ($class)
				print " " . $class;
			?>" toclevel="<?php
			print $level; ?>" toclink="<?php print $tag; ?>"><?php
			print $title . "</h" . $level . ">";
	}

	public static function file_quote($file, $download = TRUE) {
		?>
		<p><pre><?php require $file; ?></pre></p>
		<?php
		if ($download) {
			?>
			<p><a href="<?php print $file; ?>" download="<?php
				print basename($file); ?>">Download</a></p>
			<?php
		}
	}
}
