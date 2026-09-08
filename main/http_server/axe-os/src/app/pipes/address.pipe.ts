import { Pipe, PipeTransform } from '@angular/core';

@Pipe({
  name: 'address',
  pure: true,
})
export class AddressPipe implements PipeTransform {
  private static _this = new AddressPipe();

  public static transform(value: string, args?: { length?: number }): string {
    return this._this.transform(value, args);
  }

  transform(value: string, args?: { length?: number }): string {
    if (!value) return value;

    const maxLength = args?.length ?? 22;

    let segments: string[] = [];
    let segmentStartIdx = 0;
    let colonIndex = value.indexOf(':');
    if (colonIndex != -1) {
      segmentStartIdx = colonIndex + 1;
      segments.push(value.slice(0, colonIndex + 1));
    }
    for (let i = segmentStartIdx; i < value.length; i += 4) {
      segments.push(value.slice(i, i + 4));
    }

    let formatted = segments.join(' ');
    if (formatted.length <= maxLength) return formatted;

    const mid = Math.ceil(segments.length / 2);
    let left = segments.slice(0, mid);
    let right = segments.slice(mid);
    do {
      if (left.length > right.length) left.pop();
      else right.shift();
      formatted = left.join(' ') + '...' + right.join(' ');
    } while (
      formatted.length > maxLength &&
      (left.length > 1 || right.length > 1)
    );

    return formatted;
  }
}
