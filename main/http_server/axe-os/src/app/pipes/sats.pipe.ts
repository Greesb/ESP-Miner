import { Pipe, PipeTransform } from '@angular/core';

@Pipe({
  name: 'sats',
  pure: true,
})
export class SatsPipe implements PipeTransform {
  private static _this = new SatsPipe();

  public static transform(
    value: number,
    coin?: string,
    decimalPlaces?: number,
    valueDivider?: number,
  ): string {
    return this._this.transform(value, coin, decimalPlaces, valueDivider);
  }

  transform(
    value: number,
    coin?: string,
    decimalPlaces?: number,
    valueDivider?: number,
  ): string {
    if (!value) return '0 ' + (coin ?? 'BTC');
    return (
      (value / (valueDivider ?? 100_000_000)).toFixed(decimalPlaces ?? 8) +
      ' ' +
      (coin ?? 'BTC')
    );
  }
}
