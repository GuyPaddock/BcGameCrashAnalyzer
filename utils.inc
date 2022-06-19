<?php
/**
 * @file
 * Common utility functions used by the scripts in this package.
 */

/**
 * Reads-in historical results data from past BC.Game "Crash" games.
 *
 * @return array[]
 *   An array of arrays containing result data, sorted in ascending order by
 *   Game ID. Each array contains the following keys:
 *     - Game ID: The unique ID of the game.
 *     - Result: The amount of the actual payout for that round.
 */
function readResultData(): array {
  // NOTE: Sadly, this is not a continuous data set. Data was captured manually
  // and there are two gaps in the data.
  $dataFile = fopen('bc_game_data_4905002_4923003_with_gaps.csv', 'r');

  $rowNumber = 0;
  $headings  = [];
  $results   = [];

  while (($data = fgetcsv($dataFile)) !== FALSE) {
    if ($rowNumber === 0) {
      $headings = $data;

      ++$rowNumber;
    }
    else {
      $resultRow =
        array_combine($headings, $data);

      // Remove "x" at the end of each payout.
      $resultRow['Result'] = str_replace('x', '', $resultRow['Result']);

      $results[] = $resultRow;
    }
  }

  fclose($dataFile);

  // Sort from lowest to highest game ID.
  usort(
    $results,
    fn(array $a, array $b): int => $a['Game ID'] <=> $b['Game ID']
  );

  return $results;
}

/**
 * Simulates a simple betting strategy of betting the same amount each time.
 *
 * The simulation output is printed to standard output.
 *
 * @param float $startingAmount
 *   The amount that the player starts with in their wallet (in USD).
 * @param float $targetPayout
 *   The payout at which to cash out when Crash reaches it.
 * @param float $betAmount
 *   The amount to wager each time.
 * @param array $results
 *   The historical data over which to run the simulation.
 *
 * @return float
 *   The ending amount of money in the player's wallet after simulating over the
 *   result set. Can be 0.00 if the player is bankrupt.
 */
function simulateSimpleBetting(float $startingAmount,
                               float $targetPayout,
                               float $betAmount,
                               array $results): float {
  echo "Simple Betting\n";
  echo "--------------\n";

  $endingAmount = $startingAmount;

  $fmt = new NumberFormatter('en_US', NumberFormatter::CURRENCY);

  print "Starting Amount: " . $fmt->formatCurrency($startingAmount, 'USD') . "\n";
  print "\n";
  print "Bet:             $" . sprintf("%2.4f", $betAmount) . "\n";
  print "Payout:          " . $targetPayout . "x\n";
  print "Max Rounds:      " . count($results) . "\n";
  print "\n";

  $min            = $startingAmount;
  $max            = $startingAmount;

  $roundsFinished = 0;
  $roundsLost     = 0;
  $roundsWon      = 0;

  $totalLossAmount        = 0;
  $lossAmountBeforeWin    = 0;
  $lossesAmountsBeforeWin = [];

  $lossCountBeforeWin  = 0;
  $lossCountsBeforeWin = [];

  foreach ($results as $result) {
    $actualPayout = $result['Result'];

    if ($betAmount > $endingAmount) {
      print "  => BUST!";
      break;
    }
    else {
      print "  BET: $" . sprintf("%2.4f", $betAmount);
      $endingAmount -= $betAmount;
    }

    if ($actualPayout >= $targetPayout) {
      $winAmount     = $betAmount * $targetPayout;
      $endingAmount += $winAmount;

      print sprintf(
        "  => WIN %2.4f: %s\n",
        $winAmount,
        $fmt->formatCurrency($endingAmount, 'USD')
      );

      ++$roundsWon;

      $lossesAmountsBeforeWin[] = $lossAmountBeforeWin;
      $lossAmountBeforeWin      = 0;

      $lossCountsBeforeWin[] = $lossCountBeforeWin;
      $lossCountBeforeWin    = 0;
    }
    else {
      print sprintf(
        "  => LOSS $%2.4f: %s\n",
        $betAmount,
        $fmt->formatCurrency($endingAmount, 'USD')
      );

      ++$roundsLost;

      $totalLossAmount     += $betAmount;
      $lossAmountBeforeWin += $betAmount;

      ++$lossCountBeforeWin;
    }

    if ($endingAmount < $min) {
      $min = $endingAmount;
    }

    if ($endingAmount > $max) {
      $max = $endingAmount;
    }

    ++$roundsFinished;
  }

  $averageLossBeforeWinAmount =
    array_sum($lossesAmountsBeforeWin) / count($lossesAmountsBeforeWin);

  $medianLossBeforeWinAmount = calculateMedian($lossesAmountsBeforeWin);

  $averageLossCountBeforeWin =
    array_sum($lossCountsBeforeWin) / count($lossCountsBeforeWin);

  $medianLossCountBeforeWin = calculateMedian($lossCountsBeforeWin);
  $minLossCountBeforeWin = min($lossCountsBeforeWin);
  $maxLossCountBeforeWin = max($lossCountsBeforeWin);

  print "\n";
  print "Starting Amount:   " . $fmt->formatCurrency($startingAmount, 'USD') . "\n";
  print "\n";
  print "Bet:               $" . sprintf("%2.4f", $betAmount) . "\n";
  print "Payout:            " . $targetPayout . "x\n";
  print "\n";
  print "Min Amount:        " . $fmt->formatCurrency($min, 'USD') . "\n";
  print "Max Amount:        " . $fmt->formatCurrency($max, 'USD') . "\n";
  print "\n";
  print "Tot Loss Amount:   " . $fmt->formatCurrency($totalLossAmount, 'USD') . "\n";
  print "Avg Loss B4 Win:   " . $fmt->formatCurrency($averageLossBeforeWinAmount, 'USD') . "\n";
  print "Med Loss B4 Win:   " . $fmt->formatCurrency($medianLossBeforeWinAmount, 'USD') . "\n";
  print "\n";
  print "Rounds Finished:   " . $roundsFinished . "\n";
  print "Rounds Won:        " . $roundsWon . "\n";
  print "Rounds Lost:       " . $roundsLost . "\n";
  print "Avg Losses B4 Win: " . $averageLossCountBeforeWin . "\n";
  print "Med Losses B4 Win: " . $medianLossCountBeforeWin . "\n";
  print "Min Losses B4 Win: " . $minLossCountBeforeWin . "\n";
  print "Max Losses B4 Win: " . $maxLossCountBeforeWin . "\n";
  print "\n";
  print "Ending Amount:     " . $fmt->formatCurrency($endingAmount, 'USD') . "\n";
  print "\n\n";

  return $endingAmount;
}

/**
 * Simulates a simple betting strategy of betting the same amount each time.
 *
 * The simulation output is printed to standard output.
 *
 * @param float $startingAmount
 *   The amount that the player starts with in their wallet (in USD).
 * @param float $targetPayout
 *   The payout at which to cash out when Crash reaches it.
 * @param float $betAmount
 *   The amount to wager each time.
 * @param array $results
 *   The historical data over which to run the simulation.
 *
 * @return float
 *   The ending amount of money in the player's wallet after simulating over the
 *   result set. Can be 0.00 if the player is bankrupt.
 */
function simulateSimpleBettingCsv(float $startingAmount,
                                  float $targetPayout,
                                  float $betAmount,
                                  array $results): float {
  $endingAmount = $startingAmount;

  fputcsv(STDOUT, [
    'Game ID',
    'Bet',
    'Target Payout',
    'Actual Payout',
    'Net',
    'Balance',
    'Note',
  ]);

  foreach ($results as $result) {
    if ($betAmount > $endingAmount) {
      $endingAmount = 0;
      break;
    }
    else {
      $endingAmount -= $betAmount;
    }

    if ($result['Result'] >= $targetPayout) {
      $winAmount     = $betAmount * $targetPayout;
      $endingAmount += $winAmount;

      fputcsv(
        STDOUT,
        [
          $result['Game ID'],
          $betAmount,
          $targetPayout,
          $result['Result'],
          $winAmount,
          $endingAmount,
          'Win'
        ]
      );
    }
    else {
      fputcsv(
        STDOUT,
        [
          $result['Game ID'],
          $betAmount,
          $targetPayout,
          $result['Result'],
          -$betAmount,
          $endingAmount,
          'Loss'
        ]
      );
    }
  }

  return $endingAmount;
}

/**
 * Simulates a simple betting strategy of betting the same amount each time.
 *
 * No output is printed during the simulation; this version is used internally
 * by scripts to find viable candidates.
 *
 * @param float $startingAmount
 *   The amount that the player starts with in their wallet (in USD).
 * @param float $targetPayout
 *   The payout at which to cash out when Crash reaches it.
 * @param float $betAmount
 *   The amount to wager each time.
 * @param array $results
 *   The historical data over which to run the simulation.
 *
 * @return float
 *   The ending amount of money in the player's wallet after simulating over the
 *   result set. Can be 0.00 if the player is bankrupt.
 */
function simulateSimpleBettingQuiet(float $startingAmount,
                                    float $targetPayout,
                                    float $betAmount,
                                    array $results): float {
  $endingAmount = $startingAmount;

  $min            = $startingAmount;
  $max            = $startingAmount;
  $roundsFinished = 0;

  foreach ($results as $result) {
    $actualPayout = $result['Result'];

    if ($betAmount > $endingAmount) {
      break;
    }
    else {
      $endingAmount -= $betAmount;
    }

    if ($actualPayout >= $targetPayout) {
      $winAmount     = $betAmount * $targetPayout;
      $endingAmount += $winAmount;
    }

    if ($endingAmount < $min) {
      $min = $endingAmount;
    }

    if ($endingAmount > $max) {
      $max = $endingAmount;
    }

    ++$roundsFinished;
  }

  return $endingAmount;
}

/**
 * Simulates a Martingale betting strategy of increasing bets on losses.
 *
 * In theory, Martingale would always result in a long-term win for players with
 * infinite wealth. Unfortunately, players do not have infinite wealth, so this
 * is guaranteed to bankrupt all players in the long term even if it generates
 * short-term gains.
 *
 * The simulation output is printed to standard output.
 *
 * @param float $startingAmount
 *   The amount that the player starts with in their wallet (in USD).
 * @param float $targetPayout
 *   The payout at which to cash out when Crash reaches it.
 * @param float $startingBetAmount
 *   The amount of the first wager. Wagers also return to this value on each
 *   win.
 * @param float $lossMultiplier
 *   The amount to multiply losses when
 * @param array $results
 *   The historical data over which to run the simulation.
 *
 * @return float
 *   The ending amount of money in the player's wallet after simulating over the
 *   result set. Can be 0.00 if the player is bankrupt.
 */
function simulateMartingaleBetting(float $startingAmount,
                                   float $targetPayout,
                                   float $startingBetAmount,
                                   float $lossMultiplier,
                                   array $results): float {
  echo "Martingale Betting\n";
  echo "------------------\n";

  $endingAmount = $startingAmount;
  $betAmount    = $startingBetAmount;

  $fmt = new NumberFormatter('en_US', NumberFormatter::CURRENCY);

  print "Starting Amount: " . $fmt->formatCurrency($startingAmount, 'USD') . "\n";
  print "\n";
  print "Bet:             " . $fmt->formatCurrency($startingBetAmount, 'USD') . "\n";
  print "Payout:          " . $targetPayout . "x\n";
  print sprintf("Loss Multiplier: %0.2f\n", $lossMultiplier);
  print "Max Rounds:      " . count($results) . "\n";
  print "\n";

  $min            = $startingAmount;
  $max            = $startingAmount;
  $roundsFinished = 0;

  foreach ($results as $result) {
    $actualPayout = $result['Result'];

    if ($betAmount > $endingAmount) {
      print "  => BUST!";
      break;
    }
    else {
      print "  BET: " . $fmt->formatCurrency($betAmount, 'USD');
      $endingAmount -= $betAmount;
    }

    if ($actualPayout >= $targetPayout) {
      $winAmount     = $betAmount * $targetPayout;
      $endingAmount += $winAmount;

      $betAmount = $startingBetAmount;

      print sprintf(
        "  => WIN  %s: %s\n",
        $fmt->formatCurrency($winAmount, 'USD'),
        $fmt->formatCurrency($endingAmount, 'USD')
      );
    }
    else {
      $betAmount *= $lossMultiplier;

      print sprintf(
        "  => LOSS %s: %s\n",
        $fmt->formatCurrency($betAmount, 'USD'),
        $fmt->formatCurrency($endingAmount, 'USD')
      );
    }

    if ($endingAmount < $min) {
      $min = $endingAmount;
    }

    if ($endingAmount > $max) {
      $max = $endingAmount;
    }

    ++$roundsFinished;
  }

  print "\n";
  print "Starting Amount: " . $fmt->formatCurrency($startingAmount, 'USD') . "\n";
  print "\n";
  print "Bet:             " . $fmt->formatCurrency($startingBetAmount, 'USD') . "\n";
  print "Payout:          " . $targetPayout . "x\n";
  print sprintf("Loss Multiplier: %0.2f\n", $lossMultiplier);
  print "Rounds Finished: " . $roundsFinished . "\n";
  print "\n";
  print "Min Amount:      " . $fmt->formatCurrency($min, 'USD') . "\n";
  print "Max Amount:      " . $fmt->formatCurrency($max, 'USD') . "\n";
  print "\n";
  print "Ending Amount:   " . $fmt->formatCurrency($endingAmount, 'USD') . "\n";
  print "\n\n";

  return $endingAmount;
}

/**
 * Calculates the median value of an array.
 *
 * From:
 * https://thisinterestsme.com/php-calculate-median/
 *
 * @param int[] $values
 *   The values you want to get the median value of.
 *
 * @return int
 *   The median of the values.
 */
function calculateMedian(array $values) {
  if (empty($values)) {
    return false;
  }

  // Count how many elements are in the array.
  $num = count($values);

  // Determine the middle value of the array.
  $middleVal = floor(($num - 1) / 2);

  // If the size of the array is an odd number, then the middle value is the
  // median.
  if ($num % 2) {
    return $values[$middleVal];
  }
  else {
    // If the size of the array is an even number, then we have to get the two
    // middle values and get their average. The $middleVal var will be the low
    // end of the middle
    $lowMid  = $values[$middleVal];
    $highMid = $values[$middleVal + 1];

    // Return the average of the low and high.
    return (($lowMid + $highMid) / 2);
  }
}
