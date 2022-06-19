<?php
/**
 * @file
 * Analyze the observed probability of a win with a particular target payout.
 *
 * The printed results show the probability of their being a payout greater than
 * or equal to the payout indicated. So, for example, output like:
 * ```
 *   [1]    => 100
 *   [1.25] => 79.284963196635
 *   [1.3]  => 76.14791447599
 * ```
 *
 * This means:
 * 1. There is a 100% probability of a payout >= 1.00.
 * 2. There is a 79.28% probability of a payout >= 1.25.
 * 3. There is a 76.15% probability of a payout >= 1.30.
 */
require_once('utils.inc');

//==============================================================================
// Main Script
//==============================================================================
$results = readResultData();

$appearances = [];

foreach ($results as $result) {
  $payout = $result['Result'];

  if (!isset($appearances[$payout])) {
    $appearances[$payout] = 1;
  }
  else {
    $appearances[$payout]++;
  }
}

ksort($appearances);

$total_appearances = array_sum($appearances);
$appearances_gteq  = [];

foreach ($appearances as $payout => $count) {
  $payout_appearances = 0;

  foreach ($appearances as $otherPayout => $otherCount) {
    if ($otherPayout >= $payout) {
      $payout_appearances += $otherCount;
    }
  }

  $appearances_gteq[$payout] = $payout_appearances / $total_appearances * 100;
}

print_r($appearances_gteq);
