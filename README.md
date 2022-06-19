# BC.Game "Crash" Analysis Scripts
A collection of scripts to analyze game data and payouts for BC.Game's "Crash" 
game.

## Contents
- `analyze.php` - A script to take in a range of bet amounts and payouts, select
  the highest-yield combination of bet and payout, and display simulation
  results on standard output.
- `analyze.php` - A script to take in a range of bet amounts and payouts, select
  the highest-yield combination of bet and payout, and display simulation
  results on standard output in CSV format.
- `analyze_cdf.php` - A script to analyze the probability that a particular 
  payout will result in a win.
- `Stop-Loss Martingale.js` - An "advanced" BC.Game JavaScript file for 
  implementing a variation on Martingale that resets to the base bet upon
  reaching a threshold maximum bet amount. The intent of this script was to 
  avoid a Martingale strategy that's starting out with bets of $0.005 suddenly
  placing a bet for $81.92 after 14 consecutive losses. In reality, though this
  can prevent a catastrophic bankruptcy in the short term, limiting the loss 
  just serves to ensure the player loses money over time and will still
  eventually become bankrupt.

## Note about Data
This repo includes data collected from "Crash" games 4905002 through 4923003,
with some unfortunate gaps. The data was collected manually (via manual copy &
paste) from the BC.Game interface, so gaps were the result of waiting too long
to grab the next set of data.

Feel free to grab your own data. The process followed was:
1.  Open a new Excel workbook.
2.  In Crash, click the "Results" tab at the bottom of the page.
3.  Select the entire table (make sure it's all selected or you'll only end up
    with jumbled text).
4.  Paste the table you copied into Excel in the first empty row, using the 
    "Match Destination Formatting" function to eliminate the unwanted HTML
    output.
5.  Click "Next" in the "Results" table to go the next page of 20 results.
6.  (If necessary) Re-select the table, if it is not already selected. (If you 
    select the table just right, Edge/Chrome won't clear your selection when the
    result data changes).
7.  Repeat steps 4-6 until you've captured all 2,000 results.
8.  Back in Excel, use the Sort function to sort from lowest to highest by Game
    ID.
9.  Delete the third column, which usually just contains the text "Verify".
10. Use the "Remove Duplicates" feature to remove any values that may be
    duplicated. (This removes multiple instances of the "Game ID", "Result",
    and "Hash" heading, as well as any numbers that you copied more than once
    because BC.Game added results to the table while you were copying).
11. Eliminate all but the first copy of the "Game ID", "Result", and "Hash" 
    heading. This should only be in the very first row.
12. Save the file in CSV format.
