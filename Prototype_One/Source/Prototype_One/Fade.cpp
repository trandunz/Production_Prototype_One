// Fill out your copyright notice in the Description page of Project Settings.


#include "Fade.h"

Fade::Fade(UImage* _imageToFade, float _fadeTime, float _fadeStartTime)
{
	imageToFade = _imageToFade;
	fadeTime = _fadeTime;
	fadeTimer = _fadeStartTime;
}

void Fade::Tick(float deltaTime)
{
	switch (fadeState)
	{
	case FadeState::FadeIn:
		if (fadeTimer != fadeTime)
		{
			fadeTimer += deltaTime;
			if (fadeTimer > fadeTime)
			{
				fadeTimer = fadeTime;
			}
		}
		fadeValue = fadeTimer / fadeTime;
		imageToFade->SetOpacity(fadeValue);
		if (fadeValue == 1)
		{
			fadeState = FadeState::Halt;
		}
		break;
	case FadeState::FadeOut:
		if (fadeTimer != fadeTime)
		{
			fadeTimer += deltaTime;
			if (fadeTimer > fadeTime)
			{
				fadeTimer = fadeTime;
			}
		}
		fadeValue = fadeTimer / fadeTime;
		imageToFade->SetOpacity(1 - fadeValue);
		if (fadeValue == 1)
		{
			fadeState = FadeState::Halt;
		}
		break;
	case FadeState::Halt:
		break;
	}
}

void Fade::FadeIn()
{
	if (fadeTimer == fadeTime)
	{
		fadeTimer = 0;
	}
	fadeTimer = fadeValue * fadeTime;
	fadeState = FadeState::FadeIn;
}

void Fade::FadeOut()
{
	if (fadeTimer == fadeTime)
	{
		fadeTimer = 0;
	}
	fadeTimer = 1 - fadeValue * fadeTime;
	fadeState = FadeState::FadeOut;
}

void Fade::StopFade()
{
	fadeState = FadeState::Halt;
}

void Fade::ImmediateFadeOut()
{
	fadeState = FadeState::Halt;
	fadeTimer = fadeTime;
	fadeValue = 0;
	imageToFade->SetOpacity(fadeValue);
}

void Fade::ImmediateFadeIn()
{
	fadeState = FadeState::Halt;
	fadeTimer = fadeTime;
	fadeValue = 1;
	imageToFade->SetOpacity(fadeValue);
}

Fade::~Fade()
{
}
