/*
* C++ Assembler Interpreter
* Copyright (C) 2016 Chair of Computer Architecture
* at Technical University of Munich
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
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "core/memory-access.hpp"

#include <chrono>
#include <iostream>

void MemoryAccess::sleep(std::chrono::milliseconds sleepDuration) const {
  std::chrono::milliseconds sleepRemaining(sleepDuration);
  std::chrono::milliseconds sleepInterval(100);
  auto sleepStart = std::chrono::high_resolution_clock::now();
  auto sleepEnd = sleepStart;
  while (sleepRemaining > std::chrono::milliseconds::zero() &&
         !_stopFlag->load()) {
    if (sleepRemaining > sleepInterval) {
      // there is more sleep remaining than the standard duration
      std::this_thread::sleep_for(sleepInterval);
    } else {
      // sleep for the remaining duration
      std::this_thread::sleep_for(sleepRemaining);
    }
    sleepEnd = std::chrono::high_resolution_clock::now();
    // update the sleep duration
    std::chrono::milliseconds deltaSleep =
        std::chrono::duration_cast<std::chrono::milliseconds>(sleepEnd -
                                                              sleepStart);
    sleepRemaining -= deltaSleep;
    sleepStart = sleepEnd;
  }
}
