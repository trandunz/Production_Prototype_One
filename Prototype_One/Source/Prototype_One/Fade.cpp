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
			imageToFade->SetVisibility(ESlateVisibility::Hidden);
		}
		break;
	case FadeState::Halt:
		break;
	}
}

void Fade::FadeIn()
{
	imageToFade->SetVisibility(ESlateVisibility::Visible);
	fadeTimer = 0;
	fadeState = FadeState::FadeIn;
}

void Fade::FadeOut()
{
	imageToFade->SetVisibility(ESlateVisibility::Visible);
	fadeTimer = 0;
	fadeState = FadeState::FadeOut;
}

void Fade::StopFade()
{
	fadeState = FadeState::Halt;
}

void Fade::ImmediateFadeOut()
{
	imageToFade->SetVisibility(ESlateVisibility::Visible);
	fadeState = FadeState::Halt;
	fadeTimer = fadeTime;
	fadeValue = 0;
	imageToFade->SetOpacity(fadeValue);
}

void Fade::ImmediateFadeIn()
{
	imageToFade->SetVisibility(ESlateVisibility::Visible);
	fadeState = FadeState::Halt;
	fadeTimer = fadeTime;
	fadeValue = 1;
	imageToFade->SetOpacity(fadeValue);
}

Fade::~Fade()
{
}
