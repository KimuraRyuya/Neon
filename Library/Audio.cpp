#include "Audio.h"

CXAudio2Origin::CXAudio2Origin()
{
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	lpXAudio = nullptr;
	UINT32 flags = 0;
//#if (_WIN32_WINNT < 0x0602 /*_WIN32_WINNT_WIN8*/) && defined(_DEBUG)
//	flags |= XAUDIO2_DEBUG_ENGINE;
//#endif
	HRESULT hr = XAudio2Create(&lpXAudio, flags);
	assert(!hr && "XAudio2Create Error");

//#if (_WIN32_WINNT >= 0x0602 /*_WIN32_WINNT_WIN8*/) && defined(_DEBUG)
//	// To see the trace output, you need to view ETW logs for this application:
//	//    Go to Control Panel, Administrative Tools, Event Viewer.
//	//    View->Show Analytic and Debug Logs.
//	//    Applications and Services Logs / Microsoft / Windows / XAudio2. 
//	//    Right click on Microsoft Windows XAudio2 debug logging, Properties, then Enable Logging, and hit OK 
//	XAUDIO2_DEBUG_CONFIGURATION debug = { 0 };
//	debug.TraceMask = XAUDIO2_LOG_ERRORS | XAUDIO2_LOG_WARNINGS;
//	debug.BreakMask = XAUDIO2_LOG_ERRORS;
//	lpXAudio->SetDebugConfiguration(&debug, 0);
//#endif

	lpMasterVoice = nullptr;
	hr = lpXAudio->CreateMasteringVoice(&lpMasterVoice);
	assert(!hr && "CreateMasteringVoice Error");
}

CXAudio2::CXAudio2(const wchar_t* fileName, int type)
{
	HRESULT hr = S_OK;
	// ‘¶ÝŠm”F
	int ch = _waccess(fileName, 0);
	assert(!ch && "The WAVE file was not found");

	size_t size = sizeof(fileName) / sizeof( *fileName );

	wmemcpy(fName, fileName, size);

	IXAudio2SourceVoice *sv;
	hr = pLoadWAV->LoadWAVAudioFromFileEx(fileName, waveFile, waveData);
	assert(!hr && "LoadWAVAudioFromFileEx	Error");

	hr = pCXAudio2Origin->lpXAudio->CreateSourceVoice(&sv, waveData.wfx);
	assert(!hr && "CreateSourceVoice	Error");

	lpSourceVoice.emplace_back(sv);

	buffer.pAudioData = waveData.startAudio;
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	buffer.AudioBytes = waveData.audioBytes;

	switch (type)
	{
	case Type::normal:
		buffer.LoopBegin = 0;
		buffer.LoopLength = 0;
		buffer.LoopCount = 0;
		break;
	case Type::loop:
		buffer.LoopBegin = waveData.loopStart;
		buffer.LoopLength = waveData.loopLength;
		buffer.LoopCount = XAUDIO2_LOOP_INFINITE;
		break;
	default:
		break;
	}

	assert(!waveData.seek && "waveData.seek");

	hr = lpSourceVoice.front()->SubmitSourceBuffer(&buffer);
	assert(!hr && "SubmitSourceBuffer	Error");
}

bool CXAudio2::PlayWAV()
{
	for (auto& it : lpSourceVoice)
	{
		if (!playing)
		{
			it->Start();
			playing = true;
		}
		else
		{
			Copy();
			break;
		}
	}

	return true;
}

bool CXAudio2::PauseWAV()
{
	for (auto& it : lpSourceVoice)
	{
		it->Stop();
		playing = false;
	}

	return true;
}

bool CXAudio2::StopWAV()
{
	for (auto& it : lpSourceVoice)
	{
		it->Stop();
		it->FlushSourceBuffers();
		it->SubmitSourceBuffer(&buffer);
		playing = false;
	}

	return true;
}

bool CXAudio2::Update()
{
	HRESULT hr = S_OK;

	int i = 0;
	for (auto& it : lpSourceVoice)
	{
		if (it && playing)
		{
			BOOL isRunning = TRUE;
			if (SUCCEEDED(hr) && isRunning)
			{
				XAUDIO2_VOICE_STATE state;
				it->GetState(&state);
				isRunning = (state.BuffersQueued > 0) != 0;
			}
		}

		i++;
	}
	return true;
}

bool CXAudio2::Copy()
{
	HRESULT hr = S_OK;

	IXAudio2SourceVoice *sv;

	/*hr = pLoadWAV->LoadWAVAudioFromFileEx(fName, waveFile, waveData);
	assert(!hr && "LoadWAVAudioFromFileEx	Error");*/

	hr = pCXAudio2Origin->lpXAudio->CreateSourceVoice(&sv, waveData.wfx);
	assert(!hr && "CreateSourceVoice	Error");

	hr = sv->SubmitSourceBuffer(&buffer);
	assert(!hr && "SubmitSourceBuffer	Error");

	playing = true;

	lpSourceVoice.emplace_back(sv);

	lpSourceVoice.back()->Start();

	return true;
}

bool CXAudio2::SetVolume(float volume)
{
	float newVolume = volume;

	if (volume < 0.0f) newVolume = 0.0f;
	else if (1.0f < volume) newVolume = 1.0f;

	for (auto& it : lpSourceVoice)
	{
		if (it && playing)
		{
			it->SetVolume(newVolume, 0);
		}
	}

	return true;
}

bool CXAudio2::SetPitch(float pitch)
{
	float newPitch = pitch;

	if (pitch < 0.0f) newPitch = 0.0f;
	else if (2.0f < pitch) newPitch = 2.0f;

	for (auto& it : lpSourceVoice)
	{
		if (it && playing)
		{
			it->SetFrequencyRatio(newPitch);
		}
	}

	return true;
}

float CXAudio2::GetVolume()
{
	float v = 0.0f;
	for (auto& it : lpSourceVoice)
	{
		if (it && playing)
		{
			it->GetVolume(&v);
		}
	}

	return v;
}

float CXAudio2::GetPitch()
{
	XAUDIO2_FILTER_PARAMETERS p;
	for (auto& it : lpSourceVoice)
	{
		if (it && playing)
		{
			it->GetFilterParameters(&p);
			return p.Frequency;
		}
	}
	return 0.0f;

}