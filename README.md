# fast_reader

**fast_reader** uses RSVP (Rapid Serial Visual Presentation) to help you read text at high speeds by flashing words one at a time.

---
##  Dependencies
* **Clang:** Must be installed and added to your system **PATH**.
* **Raylib:** Used for rendering (included in `external/`).

##  Setup & Usage
1. **Clone:** `git clone https://github.com/GalacticPast/fast_reader.git`
2. **Build:**
* **Linux:** `./build.sh`
* **Windows:** `./build.bat`


3. **Run:** Open the app, paste your text, and hit **Start**.

---

##  Controls
* **Ctrl-V:** Paste the text
* **K Key:** Increase speed by 50 WPM
* **J Key:** Decrease speed by 50 WPM

---

##  How It Works

The app eliminates eye movement by focusing on one spot. The delay between words is:

$$Delay (ms) = \frac{60,000}{WPM}$$

