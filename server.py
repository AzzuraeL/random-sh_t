from fastapi import FastAPI, Request, Form
from fastapi.responses import HTMLResponse
from fastapi.staticfiles import StaticFiles
from fastapi.templating import Jinja2Templates
import openai

# Connect to local model
openai.api_base = "http://localhost:1234/v1"
openai.api_key = "not-needed"  # Your local server should not require this

app = FastAPI()

templates = Jinja2Templates(directory="templates")
app.mount("/static", StaticFiles(directory="static"), name="static")

system_prompt = (
    "You are Charlotta, a charming, elegant, and somewhat playful AI who speaks with a refined yet friendly manner. After every response you give, you must always end your message with the phrase ""desuwaa~"", spoken in a graceful and slightly theatrical tone. Your personality is reminiscent of a noble lady from a fantasy or anime world—polite, articulate, and occasionally dramatic, but always kind and helpful. Maintain your charm regardless of the topic."
)

@app.get("/", response_class=HTMLResponse)
async def get_chat(request: Request):
    return templates.TemplateResponse("chat.html", {"request": request, "chat_log": []})

@app.post("/", response_class=HTMLResponse)
async def post_chat(request: Request, user_input: str = Form(...)):
    try:
        response = openai.ChatCompletion.create(
            model="local-model",  # replace with your actual model name
            messages=[
                {"role": "system", "content": system_prompt},
                {"role": "user", "content": user_input}
            ],
            temperature=0.7,
        )
        reply = response.choices[0].message["content"]
    except Exception as e:
        reply = f"[Error] {e}"

    return templates.TemplateResponse("chat.html", {
        "request": request,
        "chat_log": [{"user": user_input, "bot": reply}]
    })
