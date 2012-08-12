/*
 *   $Id$
 *	 Reloded an Amstrad CPC emulator
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
#include "config_manager.h"
#include "input.h"
#include "fdc.h"
#include "psg.h"
#include "ppi.h"
#include "tape.h"
#include "z80.h"

#include "audioPlugin.h"

#include "timer.h"
#include "crtc.h"

#include <iostream>
#include <semaphore.h>

#include "synchro.h"

#ifdef USE_PTHREAD
#include <pthread.h>
#endif

class t_z80regs;
class t_VDU;
class t_GateArray;
class t_CRTC;
class t_Memory;

#ifdef WITH_ASM
#include "../asm/CapASM.h"
#endif

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

	t_GateArray				_gateArray;
	t_VDU					_vdu;
	t_CRTC					*_crtc;
	t_FDC					_fdc;
	t_PPI					_ppi;
	t_Tape					_tape;
	t_PSG					_psg;
	t_Memory				_cpcMemory;
	t_z80regs				_z80;

	unsigned int			_cycleCount;
	AudioPlugin&			_audioPlugin;

#ifdef USE_PTHREAD
	pthread_t emuthread;
#endif

	/**
	 * Address to jump if required
	 */
	int goToAddress ;

	Emulator(VideoPlugin& video, AudioPlugin& audio);
	virtual ~Emulator();
	static Emulator* instance;

public:
	static Emulator* getInstance();
	void setCRTC(t_CRTC* newCRTC) {
		this->Pause();
		delete _crtc;
		_crtc = newCRTC;
		_crtc->Reset();
		this->GetZ80().setCRTC(newCRTC);
		this->Run();
	}

	bool Init();
	void Loop();
	void Emulate();

	bool isInit() const {return _isInit;}

	//## These are events sent from the GUI to the emulator
	/**
	 * Operate the keyboard emulation
   	 * Specific of the GUI.
	 */
	virtual void PressKey(uint32_t key, uint32_t mod) = 0;
	virtual void ReleaseKey(uint32_t key, uint32_t mod) = 0;


	//## These are events sent from the emulator to the GUI
	// This way the GUI does not have to poll the emu to know these things

	// screen update every 50Hz (do we want that ?)

	// fdc led status
	virtual void fdcLed(bool on) {};
	virtual void fdcNotifyRead(int side, int track, t_sector* sector) {};
		// mode > 0 : read sector
		// mode < 0 : read track
		// mode = 0 : sector not found


	//## These are "internal" methods.
	// They still may be overriden from the gui to do some things more.

	virtual void getConfigPath(char* buf) {
      strcpy(buf,_config_path);
  	}


	virtual void logMessage(const char* message) {
		printf("%s\n", message);
	}


	/**
	 * Pause the emulator
	 */
	virtual inline void Pause()
	{
		GetConfig().paused = 1;
		timer.pause();
	}

	/**
	 * Pause the emulator due to a breakpoint
	 */
	inline void Breakpoint()
	{
		Pause();
		sem_wait(&breakpointLock);
	}

	/**
	 * Run the emulator
	 */
	inline void Run()
	{
		GetConfig().paused = 0;
		GetConfig().breakpoint = 0;
		timer.start();
		sem_post(&breakpointLock);
	}

	/**
	 * Run the emulator step by step
	 */
	inline void Step()
	{
		GetConfig().paused = 0 ;
		timer.start();
		GetConfig().breakpoint = 1 ;
		sem_post(&breakpointLock);
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

	void SaveScreenshot(string filename);

	inline t_z80regs& GetZ80() { return _z80; }
	inline t_GateArray&	GetGateArray() { return _gateArray;	}
	inline t_CRTC& GetCRTC() { return *_crtc; }
	inline t_VDU& GetVDU() { return _vdu; }
	inline t_PSG& GetPSG() { return _psg; }
	inline t_Memory& GetMemory() { return _cpcMemory; }
	inline t_FDC& GetFDC() { return _fdc; }
	inline t_PPI& GetPPI() { return _ppi; }
	inline t_Tape& GetTape() { return _tape; }
	/**
	 * Get drive a
	 */
	inline t_drive& GetDriveA() {return GetFDC().GetDriveA();}
	/**
	 * Get drive b
	 */
	inline t_drive& GetDriveB() {return GetFDC().GetDriveB();}

    inline AudioPlugin& GetAudioPlugin() {return _audioPlugin;}

	 char _config_path[1024];
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
	static bool sInitOnce;

	Timer timer;

	dword dwTicks ;
	dword dwFPS ;
	dword dwFrameCount ;

	dword dwTicksOffset ;
	dword dwTicksTarget ;
	dword dwTicksTargetFPS ;

	bool _isInit;

	//int loopcon;
public:
    SysSync emuSync; // Global sync on Emulator object
private:
	sem_t breakpointLock;
	VideoPlugin& _videoPlugin;
};

#endif
