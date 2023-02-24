#include "DialogueWidget.h"

#include "HttpModule.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

FString UDialogueWidget::RandomHelloMessage{"Hi there traveller!"};

void UDialogueWidget::OnOpenAIResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful && Response->GetContentLength() > 0)
	{
		FString JsonResponse = Response->GetContentAsString();

		// Deserialize the JSON response into a struct
		FOpenAIResponse OpenAIResponse;
		TSharedPtr<FJsonObject> JsonObject;

		if (FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(JsonResponse), JsonObject))
		{
			// Deserialize the JSON object into the struct
			OpenAIResponse.id = JsonObject->GetStringField("id");

			TArray<TSharedPtr<FJsonValue>> ChoicesArray = JsonObject->GetArrayField("choices");
			for (const auto& ChoiceValue : ChoicesArray)
			{
				TSharedPtr<FJsonObject> ChoiceObject = ChoiceValue->AsObject();
				FString Text = ChoiceObject->GetStringField("text");
				OpenAIResponse.choices.Add(Text);
				UE_LOG(LogTemp, Warning, TEXT(" %s " ), *Text );
			}

			if (OpenAIResponse.choices.Num() > 0)
			{
				FString InputString = OpenAIResponse.choices[0];
				FString RegexString = "[A-Z][^\\.!?]*[\\.!?]";

				TArray<FString> Matches;
				ApplyRegex(InputString, RegexString, Matches);
				if (Matches.Num() > 0)
				{
					RandomHelloMessage = Matches[rand() % Matches.Num()];
					UE_LOG(LogTemp, Warning, TEXT("%s"), *RandomHelloMessage );
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to deserialize OpenAI JSON response: %s"), *JsonResponse);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to receive OpenAI response"));
	}
}

void UDialogueWidget::SendOpenAIRequest(const FString& _input)
{
	FString Url = "https://api.openai.com/v1/engines/text-davinci-003/completions";

	// Construct the request headers
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetVerb("POST");
	Request->SetURL(Url);
	Request->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *API_KEY));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	// Construct the request body
	FString RequestBody = "{\"prompt\":\"" + _input + """\""
							",\"max_tokens\":100"
							",""\"temperature\":0.2}";
	
	Request->SetContentAsString(RequestBody);

	// Send the request
	Request->OnProcessRequestComplete().BindStatic(&OnOpenAIResponseReceived);
	Request->ProcessRequest();
}

void UDialogueWidget::ApplyRegex(const FString& InputString, const FString& RegexString, TArray<FString>& OutMatches)
{
	// Compile the regex pattern
	FRegexPattern RegexPattern(RegexString);
	FRegexMatcher RegexMatcher(RegexPattern, InputString);

	// Loop through the matches and extract the matching sentences
	while (RegexMatcher.FindNext())
	{
		FString Match = RegexMatcher.GetCaptureGroup(0);
		OutMatches.Add(Match.TrimStartAndEnd());
	}
}

void UDialogueWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	Dialogue_NextLine->OnPressed.AddDynamic(this, &UDialogueWidget::OnNextDialogueLine);
	RandomHelloMessage = DialogueLines[0];
	ShowDialogueBox(false);
	
	if (EnableAIParaphrasing)
		SendOpenAIRequest("Paraphrase: '" + RandomHelloMessage + "' with 5 results");
}

void UDialogueWidget::ShowDialogueBox(bool _show)
{
	if (_show)
	{
		DialogueText->SetText(FText::FromString(RandomHelloMessage));
		SetVisibility(ESlateVisibility::Visible);
		CurrentDialogueIndex = 0;
		if (auto* playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
		{
			playerController->SetInputMode(FInputModeUIOnly{});
			playerController->SetIgnoreMoveInput(true);
		}
	}
	else
	{
		if (EnableAIParaphrasing)
			SendOpenAIRequest("Paraphrase: '" + RandomHelloMessage + "' with 5 results");
		
		DialogueText->SetText(FText::FromString(RandomHelloMessage));
		SetVisibility(ESlateVisibility::Hidden);
		CurrentDialogueIndex = 0;
		if (auto* playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
		{
			playerController->SetInputMode(FInputModeGameAndUI{});
			playerController->SetIgnoreMoveInput(false);
		}
	}
}

void UDialogueWidget::OnNextDialogueLine()
{
	CurrentDialogueIndex++;
	
	UE_LOG(LogTemp, Warning, TEXT("Next Dialogue Queue Please") );
	if (CurrentDialogueIndex >= DialogueLines.Num())
	{
		if (DialogueBackground->GetVisibility() == ESlateVisibility::Visible)
		{
			ShowDialogueBox(false);
		}
			
	}
	else if (CurrentDialogueIndex < DialogueLines.Num())
	{
		DialogueText->SetText(FText::FromString(DialogueLines[CurrentDialogueIndex]));
	}
}
