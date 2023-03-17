//   Copyright 2021 - 2023 wxserver - caozhanhao
//
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.
#include "wxserver.hpp"
#include <string>

int main()
{
  ws::Server server;
  server.init("config.czh",
              [](const ws::Request &req, ws::Response &res)
              {
                if (req.content == "hi")
                {
                  res.set_text("hello");
                }
                else if (req.content == "license")
                {
                  res.set_file("LICENSE");
                }
              });
  server.run();
  return 0;
}
