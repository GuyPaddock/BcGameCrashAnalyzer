<?php
/**
 * @file
 * A script to simulate the yield of simple bets over past BC.Game Crash
 * results, select the highest-yielding combination, and output the simulation
 * data to standard out in CSV format.
 *
 * This can be useful for graphing hypothetical results in Excel or Google
 * Sheets.
 */
require_once('utils.inc');

//==============================================================================
// Parameters
//==============================================================================
// How much money you start with (in USD)
$startingAmount = 15.00;

// --- Bet Simulation Parameters
// How much to increment the bet amount.
$betAmountStep = 0.005;

// Option 1: Use these parameters to find the best option in a range of bets.
$minBetAmount = 0.005;
$maxBetAmount = 0.250;

// Option 2: Set both parameters to the same value to use a fixed bet amount.
//$minBetAmount = $maxBetAmount = 0.25;

// --- Payout Simulation Parameters
$payoutStep =  0.01;

// Option 1: Use these parameters to find the best option in a range of payouts.
// Smaller payouts are more likely but less profitable.
$minPayout  =  1.00;
$maxPayout  = 25.00;

// Option 2: Set both parameters to the same value to use a fixed payout amount.
$minPayout = $maxPayout = 22.08;

// --- Simulation Scope
// A number from 0.0 to 1.0 that indicates the percentage of the known BC crash
// result set that should be examined during the simulation. For example:
//  - 1.00 for all data.
//  - 0.25 for 25% of the most recent data.
//  - 0.10 for 10% of the most recent data.
$result_scope = 0.25;

//==============================================================================
// Main Script
//==============================================================================
$all_results     = readResultData();
$all_result_size = count($all_results);
$result_size     = floor($result_scope * $all_result_size);
$results         = array_slice($all_results, $result_size);

$totalBetSteps    = ($maxBetAmount - $minBetAmount) / $betAmountStep + 1;
$totalPayoutSteps = ($maxPayout - $minPayout) / $payoutStep + 1;
$totalSteps       = $totalBetSteps * $totalPayoutSteps;

$step       = 0;
$simResults = [];

for ($betAmount = $minBetAmount; $betAmount <= $maxBetAmount; $betAmount += $betAmountStep) {
  for ($payout = $minPayout; $payout <= $maxPayout; $payout += $payoutStep) {
    fwrite(
      STDERR,
      sprintf(
        "[Bet: %07.4f/%07.4f] [Payout:%07.4f/%07.4f] %05.2f%%\r",
        $betAmount,
        $maxBetAmount,
        $payout,
        $maxPayout,
        ((($step++)/$totalSteps) * 100)
      )
    );

    $simResult = simulateSimpleBettingQuiet(
      /* Starting amount: */ $startingAmount,
      /* Payout: */          $payout,
      /* Bet amount: */      $betAmount,
      $results
    );

    if ($simResult > $startingAmount) {
      $key = implode(',', [$betAmount, $payout]);

      $simResults[$key] = $simResult;
    }
  }
}

if (empty($simResults)) {
  print "\n\n";
  print "No solution.\n\n";
  exit(2);
}

arsort($simResults, SORT_NUMERIC);

[$betAmount, $targetPayout] = explode(',', key($simResults));

simulateSimpleBettingCsv(
  /* Starting amount: */  $startingAmount,
  /* Payout: */           $targetPayout,
  /* Bet amount: */       $betAmount,
  $results
);
