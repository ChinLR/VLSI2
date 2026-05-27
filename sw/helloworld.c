// Copyright (c) 2024 ETH Zurich and University of Bologna.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0
//
// Authors:
// - Philippe Sauter <phsauter@iis.ee.ethz.ch>

#include "uart.h"
#include "print.h"
#include "config.h"

int main() {
    uart_init();
    printf("Hello World from Croc!\n");
    uart_write_flush();
    return 0;
}




// #############################################################################################################################################
// #############################################################################################################################################
// #############################################################################################################################################
// #############################################################################################################################################




// Copyright (c) 2026 ETH Zurich.
// Licensed under Solderpad Hardware License 0.51.
//
// nn_test.c — Minimal 2-layer INT8 MLP for Croc bring-up.
//
// 8 -> 4 -> 2 INT8 MLP forward pass with one hardcoded test input.
// Mirrors the accelerator's arithmetic pattern: INT8 * INT8 -> INT32
// accumulator, ReLU between layers, arithmetic right-shift and
// saturate back to INT8.
//
// Hand-computed expected output (verified in Python):
//     layer 1 acc          : [ 0, 15, -15, 15]
//     after ReLU + (>> 2)  : [ 0,  3,   0,  3]
//     layer 2 acc          : [ 6, -6]
//     final output         : [ 6, -6]
//
// Note on output: Croc's printf only supports %x. We use a local
// print_int helper (via putchar) for signed decimal output.
//
// Author: Li Ren, VLSI 2 Spring 2026.

// #include <stdint.h>
// #include "uart.h"
// #include "print.h"
// #include "config.h"

// // --- Signed-decimal helper (Croc's printf only does %x) -------------------

// static void print_int(int v) {
//     if (v < 0) { putchar('-'); v = -v; }
//     if (v == 0) { putchar('0'); return; }
//     char buf[12];
//     int n = 0;
//     while (v > 0) { buf[n++] = (char)('0' + (v % 10)); v /= 10; }
//     while (n--)   putchar(buf[n]);
// }

// // --- The network ----------------------------------------------------------

// #define DIM_IN   8
// #define DIM_H    4
// #define DIM_OUT  2
// #define SHIFT_L1 2
// #define SHIFT_L2 0

// static const int8_t w1[DIM_H * DIM_IN] = {
//      1,  0,  1,  0,  1,  0,  1,  0,
//      0,  1,  0,  1,  0,  1,  0,  1,
//      1, -1,  1, -1,  1, -1,  1, -1,
//     -1,  1, -1,  1, -1,  1, -1,  1,
// };

// static const int8_t w2[DIM_OUT * DIM_H] = {
//      1,  1,  1,  1,
//      1, -1,  1, -1,
// };

// static const int8_t x_test[DIM_IN] = {
//     10, -5, 20, 0, -10, 15, -20, 5,
// };

// // --- Cycle counter --------------------------------------------------------

// static inline uint32_t read_mcycle(void) {
//     uint32_t v;
//     asm volatile ("csrr %0, mcycle" : "=r"(v));
//     return v;
// }

// // --- Forward pass --------------------------------------------------------

// static int8_t relu_requantize(int32_t acc, int shift) {
//     if (acc < 0) acc = 0;
//     acc >>= shift;
//     if (acc > 127) acc = 127;
//     return (int8_t)acc;
// }

// static int8_t saturate_requantize(int32_t acc, int shift) {
//     acc >>= shift;            // GCC: arithmetic shift on signed int32
//     if (acc >  127) acc =  127;
//     if (acc < -128) acc = -128;
//     return (int8_t)acc;
// }

// // --- main ----------------------------------------------------------------

// int main(void) {
//     uart_init();

//     printf("nn_test: 8 -> 4 -> 2 INT8 MLP\n");

//     int8_t h[DIM_H];
//     int8_t y[DIM_OUT];

//     uint32_t t0 = read_mcycle();

//     // Layer 1: 8 -> 4, ReLU + requantize
//     for (int o = 0; o < DIM_H; ++o) {
//         int32_t acc = 0;
//         for (int i = 0; i < DIM_IN; ++i) {
//             acc += (int32_t)w1[o * DIM_IN + i] * (int32_t)x_test[i];
//         }
//         h[o] = relu_requantize(acc, SHIFT_L1);
//     }

//     // Layer 2: 4 -> 2, signed saturate
//     for (int o = 0; o < DIM_OUT; ++o) {
//         int32_t acc = 0;
//         for (int i = 0; i < DIM_H; ++i) {
//             acc += (int32_t)w2[o * DIM_H + i] * (int32_t)h[i];
//         }
//         y[o] = saturate_requantize(acc, SHIFT_L2);
//     }

//     uint32_t t1 = read_mcycle();

//     printf("h      = [");
//     print_int(h[0]); printf(", ");
//     print_int(h[1]); printf(", ");
//     print_int(h[2]); printf(", ");
//     print_int(h[3]); printf("]\n");

//     printf("output = [");
//     print_int(y[0]); printf(", ");
//     print_int(y[1]); printf("]\n");

//     printf("cycles = ");
//     print_int((int)(t1 - t0));
//     printf("\n");

//     if (y[0] == 6 && y[1] == -6) {
//         printf("PASS (expected [6, -6])\n");
//     } else {
//         printf("FAIL (expected [6, -6])\n");
//     }

//     uart_write_flush();
//     return 0;
// }




// #############################################################################################################################################
// #############################################################################################################################################
// #############################################################################################################################################
// #############################################################################################################################################




// #include <stdint.h>
// #include "config.h"  // provides UserBaseAddr as USER_DOMAIN_BASE_ADDR or similar

// // MAC unit register offsets
// #define MAC_BASE_ADDR   (0x20000000UL)   // croc_pkg::UserBaseAddr
// #define MAC_OPERAND_A   (*((volatile uint32_t *)(MAC_BASE_ADDR + 0x00)))
// #define MAC_OPERAND_B   (*((volatile uint32_t *)(MAC_BASE_ADDR + 0x04)))
// #define MAC_CONTROL     (*((volatile uint32_t *)(MAC_BASE_ADDR + 0x08)))
// #define MAC_RESULT      (*((volatile uint32_t *)(MAC_BASE_ADDR + 0x0C)))

// #define MAC_CTRL_ACCUMULATE (1u << 0)
// #define MAC_CTRL_CLEAR      (1u << 1)

// static inline void mac_clear(void) {
//     MAC_CONTROL = MAC_CTRL_CLEAR;
// }

// static inline void mac_accumulate(int8_t a, int8_t b) {
//     MAC_OPERAND_A = (uint32_t)(int32_t)a;
//     MAC_OPERAND_B = (uint32_t)(int32_t)b;
//     MAC_CONTROL   = MAC_CTRL_ACCUMULATE;
// }

// static inline int32_t mac_result(void) {
//     return (int32_t)MAC_RESULT;
// }

// // Example: dot product of two INT8 vectors of length N
// int32_t dot_product(const int8_t *a, const int8_t *b, int n) {
//     mac_clear();
//     for (int i = 0; i < n; i++) {
//         mac_accumulate(a[i], b[i]);
//     }
//     return mac_result();
// }

// // Minimal UART init — needed to signal EOC via JTAG
// static inline void uart_init_bare(void) {
//     // Baud divisor for 20 MHz / 125000 baud = 160
//     volatile uint32_t *lcr = (volatile uint32_t *)(0x03002000UL + 0x0C);
//     volatile uint32_t *dll = (volatile uint32_t *)(0x03002000UL + 0x00);
//     *lcr = 0x80;   // DLAB=1
//     *dll = 160;
//     *lcr = 0x03;   // 8N1, DLAB=0
// }

// int main(void) {
//     uart_init_bare();

//     // Test vectors
//     static const int8_t a[4] = { 1,  2,  3,  4};
//     static const int8_t b[4] = { 4,  3,  2,  1};
//     // expected: 1*4 + 2*3 + 3*2 + 4*1 = 4+6+6+4 = 20

//     int32_t result = dot_product(a, b, 4);

//     // Signal pass/fail via return code
//     // return 0 = PASS, non-zero = FAIL
//     if (result == 20) {
//         return 0;   // PASS → CORESTATUS = 0x1 → [JTAG] Simulation finished: SUCCESS
//     } else {
//         return 1;   // FAIL
//     }
// }


// #############################################################################################################################################
// #############################################################################################################################################
// #############################################################################################################################################
// #############################################################################################################################################


// #include "uart.h"
// #include "print.h"

// // ---------- cycle counter ----------
// static inline uint32_t read_mcycle(void) {
//     uint32_t v;
//     asm volatile("csrr %0, mcycle" : "=r"(v));
//     return v;
// }

// // ---------- software reference dot-product (no MAC hardware) ----------
// static int32_t dot_product_sw(const int8_t *a, const int8_t *b, int n) {
//     int32_t acc = 0;
//     for (int i = 0; i < n; i++)
//         acc += (int32_t)a[i] * (int32_t)b[i];
//     return acc;
// }

// // ---------- signed decimal print helper ----------
// static void print_int(int32_t v) {
//     if (v < 0) { uart_write('-'); v = -v; }
//     if (v == 0) { uart_write('0'); return; }
//     char buf[12];
//     int n = 0;
//     while (v > 0) { buf[n++] = (char)('0' + (v % 10)); v /= 10; }
//     while (n--) uart_write(buf[n]);
// }

// int main(void) {
//     uart_init();

//     static const int8_t a[4] = { 1,  2,  3,  4};
//     static const int8_t b[4] = { 4,  3,  2,  1};
//     // expected: 1*4 + 2*3 + 3*2 + 4*1 = 20

//     // --- Run MAC hardware version ---
//     uint32_t t0 = read_mcycle();
//     int32_t result_hw = dot_product(a, b, 4);
//     uint32_t t1 = read_mcycle();
//     uint32_t cycles_hw = t1 - t0;

//     // --- Run software reference version ---
//     uint32_t t2 = read_mcycle();
//     int32_t result_sw = dot_product_sw(a, b, 4);
//     uint32_t t3 = read_mcycle();
//     uint32_t cycles_sw = t3 - t2;

//     // --- Print results ---
//     printf("MAC  result: "); print_int(result_hw); printf("\n");
//     printf("MAC  cycles: "); print_int((int32_t)cycles_hw); printf("\n");
//     printf("SW   result: "); print_int(result_sw); printf("\n");
//     printf("SW   cycles: "); print_int((int32_t)cycles_sw); printf("\n");

//     // --- Correctness check ---
//     if (result_hw != 20) {
//         printf("FAIL: wrong result\n");
//         uart_write_flush();
//         return 1;
//     }

//     // --- MAC vs SW cycle check ---
//     // If the MAC hardware ran, cycles_hw should be <= cycles_sw.
//     // If cycles_hw > cycles_sw, the MAC likely isn't accelerating
//     // (OBI overhead with no real compute, or not connected).
//     if (cycles_hw <= cycles_sw) {
//         printf("PASS: MAC faster or equal to SW\n");
//         uart_write_flush();
//         return 0;
//     } else {
//         printf("WARN: MAC slower than SW - check hardware connection\n");
//         uart_write_flush();
//         return 2;
//     }
// }


// #############################################################################################################################################
// #############################################################################################################################################
// #############################################################################################################################################
// #############################################################################################################################################


// #include <stdint.h>
// #include "uart.h"
// #include "print.h"

// // ---------------------------------------------------------------------------
// // Cycle counter
// // ---------------------------------------------------------------------------
// static inline uint32_t read_mcycle(void) {
//     uint32_t v;
//     asm volatile("csrr %0, mcycle" : "=r"(v));
//     return v;
// }

// // ---------------------------------------------------------------------------
// // Signed decimal print helper (Croc printf only supports %x)
// // ---------------------------------------------------------------------------
// static void print_int(int32_t v) {
//     if (v < 0) { uart_write('-'); v = -v; }
//     if (v == 0) { uart_write('0'); return; }
//     char buf[12];
//     int n = 0;
//     while (v > 0) { buf[n++] = (char)('0' + (v % 10)); v /= 10; }
//     while (n--) uart_write(buf[n]);
// }

// // ---------------------------------------------------------------------------
// // MAC unit register map  (user domain base = 0x2000_0000)
// // ---------------------------------------------------------------------------
// #define MAC_BASE_ADDR       (0x20000000UL)
// #define MAC_OPERAND_A  (*((volatile uint32_t *)(MAC_BASE_ADDR + 0x00)))
// #define MAC_OPERAND_B  (*((volatile uint32_t *)(MAC_BASE_ADDR + 0x04)))
// #define MAC_CONTROL    (*((volatile uint32_t *)(MAC_BASE_ADDR + 0x08)))
// #define MAC_RESULT     (*((volatile uint32_t *)(MAC_BASE_ADDR + 0x0C)))

// #define MAC_CTRL_ACCUMULATE (1u << 0)
// #define MAC_CTRL_CLEAR      (1u << 1)

// static inline void mac_clear(void) {
//     MAC_CONTROL = MAC_CTRL_CLEAR;
// }

// static inline void mac_accumulate(int8_t a, int8_t b) {
//     MAC_OPERAND_A = (uint32_t)(int32_t)a;
//     MAC_OPERAND_B = (uint32_t)(int32_t)b;
//     MAC_CONTROL   = MAC_CTRL_ACCUMULATE;
// }

// static inline int32_t mac_read_result(void) {
//     return (int32_t)MAC_RESULT;
// }

// // ---------------------------------------------------------------------------
// // Hardware dot product  (uses MAC unit)
// // ---------------------------------------------------------------------------
// static int32_t dot_product_hw(const int8_t *a, const int8_t *b, int n) {
//     mac_clear();
//     for (int i = 0; i < n; i++)
//         mac_accumulate(a[i], b[i]);
//     return mac_read_result();
// }

// // ---------------------------------------------------------------------------
// // Software reference dot product  (pure core, no MAC unit)
// // ---------------------------------------------------------------------------
// static int32_t dot_product_sw(const int8_t *a, const int8_t *b, int n) {
//     int32_t acc = 0;
//     for (int i = 0; i < n; i++)
//         acc += (int32_t)a[i] * (int32_t)b[i];
//     return acc;
// }

// // ---------------------------------------------------------------------------
// // main
// // ---------------------------------------------------------------------------
// int main(void) {
//     uart_init();

//     // Test vectors: 1*4 + 2*3 + 3*2 + 4*1 = 4+6+6+4 = 20
//     static const int8_t a[4] = { 1,  2,  3,  4 };
//     static const int8_t b[4] = { 4,  3,  2,  1 };
//     const int32_t EXPECTED = 20;

//     // --- Hardware (MAC unit) run ---
//     uint32_t t0 = read_mcycle();
//     int32_t  result_hw = dot_product_hw(a, b, 4);
//     uint32_t cycles_hw = read_mcycle() - t0;

//     // --- Software (core) reference run ---
//     uint32_t t2 = read_mcycle();
//     int32_t  result_sw = dot_product_sw(a, b, 4);
//     uint32_t cycles_sw = read_mcycle() - t2;

//     // --- Print results ---
//     printf("MAC result: "); print_int(result_hw); printf("\n");
//     printf("MAC cycles: "); print_int((int32_t)cycles_hw); printf("\n");
//     printf("SW  result: "); print_int(result_sw); printf("\n");
//     printf("SW  cycles: "); print_int((int32_t)cycles_sw); printf("\n");

//     // --- Correctness check ---
//     if (result_hw != EXPECTED) {
//         printf("FAIL: wrong MAC result\n");
//         uart_write_flush();
//         return 1;
//     }

//     // --- Speed check ---
//     // If MAC hardware offloaded the work, cycles_hw <= cycles_sw.
//     // If MAC is not connected, writes go to the OBI error subordinate,
//     // the result register returns 0 (caught above), and cycles_hw is
//     // just OBI-write overhead — still fast but wrong result.
//     if (cycles_hw <= cycles_sw) {
//         printf("PASS: MAC correct and faster than SW\n");
//     } else {
//         printf("WARN: MAC correct but slower than SW\n");
//     }

//     uart_write_flush();
//     return 0;
// }



// #############################################################################################################################################
// #############################################################################################################################################
// #############################################################################################################################################
// #############################################################################################################################################



// #include <stdint.h>
// #include "uart.h"
// #include "print.h"

// // ---------------------------------------------------------------------------
// // Cycle counter
// // ---------------------------------------------------------------------------
// static inline uint32_t read_mcycle(void) {
//     uint32_t v;
//     asm volatile("csrr %0, mcycle" : "=r"(v));
//     return v;
// }

// // ---------------------------------------------------------------------------
// // Signed decimal print helper
// // ---------------------------------------------------------------------------
// static void print_int(int32_t v) {
//     if (v < 0) { uart_write('-'); v = -v; }
//     if (v == 0) { uart_write('0'); return; }
//     char buf[12];
//     int n = 0;
//     while (v > 0) { buf[n++] = (char)('0' + (v % 10)); v /= 10; }
//     while (n--) uart_write(buf[n]);
// }

// // ---------------------------------------------------------------------------
// // MAC unit register map (user domain base = 0x2000_0000)
// // ---------------------------------------------------------------------------
// #define MAC_BASE_ADDR      (0x20000000UL)
// #define MAC_OPERAND_A  (*((volatile uint32_t *)(MAC_BASE_ADDR + 0x00)))
// #define MAC_OPERAND_B  (*((volatile uint32_t *)(MAC_BASE_ADDR + 0x04)))
// #define MAC_CONTROL    (*((volatile uint32_t *)(MAC_BASE_ADDR + 0x08)))
// #define MAC_RESULT     (*((volatile uint32_t *)(MAC_BASE_ADDR + 0x0C)))

// #define MAC_CTRL_ACCUMULATE (1u << 0)
// #define MAC_CTRL_CLEAR      (1u << 1)

// static inline void mac_clear(void) {
//     MAC_CONTROL = MAC_CTRL_CLEAR;
// }

// static inline void mac_accumulate(int8_t a, int8_t b) {
//     MAC_OPERAND_A = (uint32_t)(int32_t)a;
//     MAC_OPERAND_B = (uint32_t)(int32_t)b;
//     MAC_CONTROL   = MAC_CTRL_ACCUMULATE;
// }

// static inline int32_t mac_read_result(void) {
//     return (int32_t)MAC_RESULT;
// }

// // ---------------------------------------------------------------------------
// // Test vectors - three separate dot products
// // expected_0: 1*4 + 2*3 + 3*2 + 4*1 = 20
// // expected_1: 2*2 + 3*3 + 4*4       = 29
// // expected_2: (-1)*1 + 2*(-2) + 3*3 = 4
// // ---------------------------------------------------------------------------
// #define NUM_TESTS 3

// static const int8_t vec_a[NUM_TESTS][4] = {
//     { 1,  2,  3,  4},
//     { 2,  3,  4,  0},
//     {-1,  2,  3,  0}
// };
// static const int8_t vec_b[NUM_TESTS][4] = {
//     { 4,  3,  2,  1},
//     { 2,  3,  4,  0},
//     { 1, -2,  3,  0}
// };
// static const int8_t vec_len[NUM_TESTS] = {4, 3, 3};
// static const int32_t expected[NUM_TESTS] = {20, 29, 4};

// // ---------------------------------------------------------------------------
// // SW: core does the multiply-accumulate itself
// // ---------------------------------------------------------------------------
// static int32_t dot_product_sw(const int8_t *a, const int8_t *b, int n) {
//     int32_t acc = 0;
//     for (int i = 0; i < n; i++)
//         acc += (int32_t)a[i] * (int32_t)b[i];
//     return acc;
// }

// // ---------------------------------------------------------------------------
// // HW: core writes operands to MAC unit, MAC unit does the computation
// // ---------------------------------------------------------------------------
// static int32_t dot_product_hw(const int8_t *a, const int8_t *b, int n) {
//     mac_clear();
//     for (int i = 0; i < n; i++)
//         mac_accumulate(a[i], b[i]);
//     return mac_read_result();
// }

// // ---------------------------------------------------------------------------
// // main
// // ---------------------------------------------------------------------------
// int main(void) {
//     uart_init();

//     printf("=== Core (SW) test ===\n");
//     uint32_t total_sw = 0;
//     int sw_pass = 1;
//     for (int t = 0; t < NUM_TESTS; t++) {
//         uint32_t t0     = read_mcycle();
//         int32_t  result = dot_product_sw(vec_a[t], vec_b[t], vec_len[t]);
//         uint32_t cycles = read_mcycle() - t0;
//         total_sw += cycles;

//         printf("  test "); print_int(t);
//         printf(": result="); print_int(result);
//         printf(" cycles="); print_int((int32_t)cycles);
//         if (result == expected[t]) {
//             printf(" PASS\n");
//         } else {
//             printf(" FAIL (expected "); print_int(expected[t]); printf(")\n");
//             sw_pass = 0;
//         }
//     }
//     printf("  SW total cycles: "); print_int((int32_t)total_sw); printf("\n");

//     printf("=== MAC (HW) test ===\n");
//     uint32_t total_hw = 0;
//     int hw_pass = 1;
//     for (int t = 0; t < NUM_TESTS; t++) {
//         uint32_t t0     = read_mcycle();
//         int32_t  result = dot_product_hw(vec_a[t], vec_b[t], vec_len[t]);
//         uint32_t cycles = read_mcycle() - t0;
//         total_hw += cycles;

//         printf("  test "); print_int(t);
//         printf(": result="); print_int(result);
//         printf(" cycles="); print_int((int32_t)cycles);
//         if (result == expected[t]) {
//             printf(" PASS\n");
//         } else {
//             printf(" FAIL (expected "); print_int(expected[t]); printf(")\n");
//             hw_pass = 0;
//         }
//     }
//     printf("  HW total cycles: "); print_int((int32_t)total_hw); printf("\n");

//     printf("=== Comparison ===\n");
//     printf("  SW total: "); print_int((int32_t)total_sw); printf(" cycles\n");
//     printf("  HW total: "); print_int((int32_t)total_hw); printf(" cycles\n");
//     if (total_hw < total_sw) {
//         printf("  MAC is faster by "); print_int((int32_t)(total_sw - total_hw));
//         printf(" cycles\n");
//     } else {
//         printf("  MAC is NOT faster (check HW connection)\n");
//     }

//     uart_write_flush();

//     if (sw_pass && hw_pass && total_hw < total_sw)
//         return 0;  // full PASS
//     else
//         return 1;  // FAIL
// }