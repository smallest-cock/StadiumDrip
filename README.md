# Stadium Drip (BakkesMod plugin)
A plugin to completely swag out your Rocket League experience 🥶💨♿💅💯🔥

peep the stadium drip my guy 👀 

deadass

## 🎥 Video Tutorial
<a href='https://youtu.be/91Uc_UaLiwY'>
  <img src='./assets/images/plugin_screenshot.png' alt="showcase video" width="600"/>
</a>

https://youtu.be/91Uc_UaLiwY

## ✨ Features
- Customize stadium colors in matches and freeplay
  - Option to use RGB effect or a single field color in freeplay
- Customize stadium ads
- Customize team names
- Customize goal scored messages
  - Can be customized based on *who* scored (you/teammate/opponent)
- Customize kickoff countdown messages
- Customize the rolling message at the bottom of menu screens (Message of the Day)
- Unlock all menu options in freeplay
  - Create private matches, LAN matches, join tournaments, etc.
- Change your car's location in main menu
- Change your FOV angle in main menu
- Change the main menu background map
- Customize the map when viewing a replay
  - Great for recording clips


## 🔧 How To Install
Find the latest version in [Releases](https://github.com/smallest-cock/StadiumDrip/releases)

1. Click `StadiumDrip.zip` to download it

2. Extract `StadiumDrip.zip`, and run `install.bat`

<br>

## 💻 Commands
| Command | Description |
|---------|:-----------:|
`sd_apply_ad_texture` | apply selected ad texture to stadium
`sd_apply_team_names` | apply custom team names
`sd_apply_motd` | apply custom rolling footer message at the bottom of menu screens
`sd_show_ball_trail` | show the ball trail effect at all times
`sd_exit_to_main_menu` | exit to the main menu
`sd_forfeit` | forfeit the current match

## 🛠️ Building
After cloning the repo, make sure to initialize the submodules before attempting to build the .sln:

Run `./scripts/init-submodules.bat` (or `./scripts/init-submodules.sh` for bash) to initialize the submodules in an optimal way

<details> <summary>🔍 Why this instead of <code>git submodule update --init --recursive</code> ?</summary>
<li>Avoids downloading 200MB of history for the <strong>nlohmann/json</strong> library</li>
<li>Allows Git to detect updates for the other submodules</li>
</details>


## ❤️ Support

<br>

[![ko-fi](https://ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/sslowdev)
