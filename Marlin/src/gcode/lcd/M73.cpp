/**
 * Marlin 3D Printer Firmware
 * Copyright (c) 2020 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (c) 2011 Camiel Gubbels / Erik van der Zalm
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#include "../../inc/MarlinConfig.h"

#if ENABLED(SET_PROGRESS_MANUALLY)

#include "../gcode.h"
#include "../../lcd/marlinui.h"
#include "../../sd/cardreader.h"
#include "../../libs/numtostr.h"

#if ENABLED(DWIN_LCD_PROUI)
  #include "../../lcd/e3v2/proui/dwin.h"
#elif ENABLED(SOVOL_SV06_RTS)
  unsigned char percentComplete = 0;
  uint16_t timeRemaining = 0;
  #include "../../lcd/sovol_rts/sovol_rts.h"
#endif

/**
 * M73: Set percentage complete (for display on LCD)
 *
 * Example:
 *   M73 P25.63 ; Set progress to 25.63%
 *   M73 R456   ; Set remaining time to 456 minutes
 *   M73 C12    ; Set next interaction countdown to 12 minutes
 *   M73        ; Report current values
 *
 * M73 Progress: ---%; Time left: -----m; Change: -----m;
 *
 * When PRINT_PROGRESS_SHOW_DECIMALS is enabled - reports percent with 100% / 23.4% / 3.45% format
 *
 */
void GcodeSuite::M73() {

  #if ENABLED(SET_PROGRESS_PERCENT)
    if (parser.seenval('P'))
      ui.set_progress((PROGRESS_SCALE) > 1
        ? parser.value_float() * (PROGRESS_SCALE)
        : parser.value_byte()
      );
#if ENABLED(SOVOL_SV06_RTS)
    percentComplete = parser.value_byte();
        rts.sendData((unsigned char)percentComplete, PRINT_PROCESS_ICON_VP);
           rts.sendData((unsigned char)percentComplete, PRINT_PROCESS_VP);
        if (percentComplete == 100)
        {
            rts.RTS_USBPrint_Finish();
        }
        else
        {
            rts.RTS_USBPrint_Set();
        }
#endif

  #endif

  #if ENABLED(SET_REMAINING_TIME)
    if (parser.seenval('R')) ui.set_remaining_time(60 * parser.value_ulong());
  #endif

  #if ENABLED(SET_INTERACTION_TIME)
    if (parser.seenval('C')) ui.set_interaction_time(60 * parser.value_ulong());
  #endif

  #if ENABLED(M73_REPORT)
    if (TERN1(M73_REPORT_SD_ONLY, IS_SD_PRINTING())) {
      SERIAL_ECHO_START();
      SERIAL_ECHOPGM(" M73");
      #if ENABLED(SET_PROGRESS_PERCENT)
        SERIAL_ECHOPGM(" Progress: ", TERN(PRINT_PROGRESS_SHOW_DECIMALS, permyriadtostr4(ui.get_progress_permyriad()), ui.get_progress_percent()), "%;");
      #endif
      #if ENABLED(SET_REMAINING_TIME)
        SERIAL_ECHOPGM(" Time left: ", ui.remaining_time / 60, "m;");
        #if ENABLED(RTS_AVAILABLE)
            timeRemaining = parser.value_ulong();
            rtscheck.((timeRemaining / 60), PRINT_SURPLUS_TIME_HOUR_VP);
            rtscheck.RTS_SndData((timeRemaining % 60), PRINT_SURPLUS_TIME_MIN_VP);
        #endif
      #endif
      #if ENABLED(SET_INTERACTION_TIME)
        SERIAL_ECHOPGM(" Change: ", ui.interaction_time / 60, "m;");
      #endif
      SERIAL_EOL();
    }
  #endif
}

#endif // SET_PROGRESS_MANUALLY
