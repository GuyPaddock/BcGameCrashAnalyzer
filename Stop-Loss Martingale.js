var config = {
  baseBet: { label: "base bet", value: 0.00000010, type: "number" },
  payout: { label: "payout", value: 5, type: "number" },
  maxBet: { label: "return to base bet if next bet >", value: 0.00024, type: "number" },
  lossMultiplier: { label: "loss multiplier", value: 3, type: "number" }
};
function main() {
  var currentBet = config.baseBet.value;
  var baseBet = config.baseBet.value;

  game.onBet = function () {
    game.bet(currentBet, config.payout.value).then(function (payout) {
      if (payout > 1) {
        currentBet = baseBet;

        log.success(
          "We won, so next bet will be " +
          currentBet +
          " " +
          currency.currencyName
        );
      }
      else {
        currentBet *= config.lossMultiplier.value;

        log.error(
          "We lost, so next bet will be " +
          currentBet +
          " " +
          currency.currencyName
        );

        if (currentBet > config.maxBet.value) {
          log.error(
            "Resetting to " + baseBet + " to avoid exceeding max bet. "
          );

          currentBet = baseBet;
        }
      }

    });
  };
}
