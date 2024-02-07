# ====================================================================================================================
# ** Unreal Pokémon created by Retro & Chill
# --------------------------------------------------------------------------------------------------------------------
# This project is intended as a means of learning more about how a game like Pokémon works by creating a framework
# from the ground up, and for non-commercial applications. While this code is original, Pokémon is the intellectual
# property of Game Freak and Nintendo, as such it is highly discouraged to use this kit to make a commercial product.
# --------------------------------------------------------------------------------------------------------------------
# THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
# THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
# TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
# ====================================================================================================================
import unittest

from pokemon.data_loader import IniData, ItemData


class TestIniData(unittest.TestCase):
    def test_get_item(self):
        ini_data = IniData("resources/items.txt")
        self.assertEqual("Repel", ini_data["REPEL"]["Name"])
        self.assertEqual("Repels", ini_data["REPEL"]["NamePlural"])
        self.assertEqual("700", ini_data["SUPERREPEL"]["Price"])
        self.assertEqual("An item that prevents weak wild Pokémon from appearing for 200 steps after its use.",
                         ini_data["SUPERREPEL"]["Description"])
        self.assertEqual("Repel,Fling_30", ini_data["MAXREPEL"]["Flags"])

    def test_iterate_items(self):
        ini_data = IniData("resources/items.txt")
        names = []
        for item in ini_data:
            names.append(item[0])

        self.assertEqual(["REPEL", "SUPERREPEL", "MAXREPEL"], names)

    def test_parse_items(self):
        item_data = ItemData("resources/items.txt")
        print(item_data.to_json())
        self.assertNotEqual('', item_data.to_json())

        with open("items.json", 'w') as file:
            file.write(item_data.to_json())


if __name__ == '__main__':
    unittest.main()
