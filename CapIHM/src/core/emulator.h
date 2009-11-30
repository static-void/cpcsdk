/*
 *   $Id$
 *	 CapriceReloded an Amstrad CPC emulator
 *   Copyright (C) 2008  cpcsdk crew
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */  

//
// Caprice32 Emulator class
//

#ifndef _EMULATOR_H_
#define _EMULATOR_H_

#include "cap32type.h"
#include "render.h"
#include "config.h"
#include "input.h"
#include "fdc.h"
#include "z80.h"
#include "audio.h"

#include <iostream>
class t_z80regs;
class t_VDU;
class t_GateArray;
class t_CRTC;
class t_PSG;
class t_PPI;
class t_Tape;
class t_Memory;

#ifdef USE_DEBUGGER
extern dword dwDebugFlag;
extern FILE *pfoDebug;
void InitDebug();
#endif

/**
 * Emulator class.
 * This class is now abstract in order to be easily adaptable 
 * with other GUI.
 */
class Emulator
{
protected:
	t_CPC					_config;
	Renderer				_renderer;
	t_Input					_input;
	t_z80regs				*_z80;
	t_VDU					*_vdu;
	t_GateArray				*_gateArray;
	t_CRTC					*_crtc;
	t_PSG					*_psg;
	t_FDC					*_fdc;
	t_PPI					*_ppi;
	t_Tape					*_tape;
	t_Memory				*_cpcMemory;
	unsigned int			_cycleCount;

	/**
	 * Address to jump if required
	 */
	int goToAddress ;

public:
	static Emulator* theEmulator;
	Emulator();
	~Emulator();

	bool Init();
	void Loop();
	void Emulate();

	// These are events sent from the GUI to the emulator
	/**
	 * Operate the keyboard emulation
   	 * Specific of the GUI.
	 */
	virtual void PressKey(uint32_t key, uint32_t mod) = 0;
	virtual void ReleaseKey(uint32_t key, uint32_t mod) = 0;


	// These are events sent from the emulator to the GUI
	// screen update every 50Hz (do we want that ?)
	// fdc led status
	// This way the GUI does not have to poll the emu to know these things

	// (nothing in this area yet)

	// These are "internal" methods.
	// They still may be overriden from the gui to do some things more.
	/**
	 * Pause the emulator
	 */
	virtual inline void Pause() { GetConfig().paused = 1; }

	/**
	 * Pause the emulator due to a breakpoint
	 */
	inline void Breakpoint()
	{
		Pause();
		GetConfig().breakpoint = 1 ;
	}
	
	/**
	 * Run the emulator
	 */
	inline void Run()
	{
		GetConfig().paused = 0;
		GetConfig().breakpoint = 0;
	}
	/**
	 * Run the emulator step by step
	 */
	inline void Step()
	{
		GetConfig().paused = 0 ;
	}


	/**
	 * Set PC at memory
	 * @param memory adress where to jump
	 */
	inline void GoTo(int memory)
	{
		goToAddress = memory ;
	}


	/**
	 * Reset computer
	 */
	void emulator_reset(bool bolMF2Reset);

	inline	t_CPC&			GetConfig()					{ return _config;		}
	/**
	 * Get the renderer of the emulator
	 */
	inline	Renderer&		GetRenderer()				{ return _renderer;		}
	/**
	 * Get the input of the emulator
	 */
	inline	t_Input&		GetInput()					{ return _input;		}

	void SaveScreenshot(string );

	/**
	 * Get the processor
	 */
	inline  t_z80regs&		GetZ80()					{ return *_z80;			}
	/**
	 * Get the gate array
	 */
	inline  t_GateArray&	GetGateArray()				{ return *_gateArray;	}
	/**
	 * Get the CRTC
	 */
	inline  t_CRTC&			GetCRTC()					{ return *_crtc;		}
	/**
	 * Get the Screen
	 */
	inline  t_VDU&			GetVDU()					{ return *_vdu;			}
	/**
	 * Get the PSG
	 */
	inline  t_PSG&			GetPSG()					{ return *_psg;			}
	/**
	 * Get the Memory
	 */
	inline  t_Memory&		GetMemory()					{ return *_cpcMemory;	}
	/**
	 * Get the FDC
	 */
	inline  t_FDC&			GetFDC()					{ return *_fdc;			}
	/**
	 * Get the ppi
	 */
	inline  t_PPI&			GetPPI()					{ return *_ppi;			}
	/**
	 * Get the tape reader
	 */
	inline  t_Tape&			GetTape()					{ return *_tape;		}
	/**
	 * Get drive a
	 */
	inline t_drive& GetDriveA() {return GetFDC().GetDriveA();}
	/**
	 * Get drive b
	 */
	inline t_drive& GetDriveB() {return GetFDC().GetDriveB();}

protected:
	bool MF2Init();

	int emulator_init();

	/**
	 * Shutdown the emu
	 */
	void emulator_shutdown();

	int printer_start();
	void printer_stop();

	/**
	 * Set PC at goToAddress
	 */
	inline void ExecGoTo()
	{
		std::cout << "Launch " << std::hex << goToAddress << std::endl ;

		//TODO check if in amsdos execution mode
		//Select bank
		GetMemory().SetROMConfig(0);
		GetMemory().ga_memory_manager();

		//Jump
		GetZ80().PC.w.l = goToAddress & 0xffff ;

		//Reset
		goToAddress = -1 ;
	}


	bool FPSDisplay;
	bool exitRequested;


	dword dwTicks ;
	dword dwFPS ;
	dword dwFrameCount ;

	dword dwTicksOffset ;
	dword dwTicksTarget ;
	dword dwTicksTargetFPS ;

};

#endif
