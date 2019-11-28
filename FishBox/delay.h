#ifndef DELAY_H_
#define DELAY_H_

#define CYCLES_PER_US 12L  //12MHz (L=Long, 64 bit)
#define CYCLES_PER_MS (CYCLES_PER_US * 1000L) //(milliseconds = mircoseconds * 1000)

#define DELAY_US(x) _delay_cycles((x * CYCLES_PER_US))
#define DELAY_MS(x) _delay_cycles((x * CYCLES_PER_MS))

#endif /* DELAY_H_ */
