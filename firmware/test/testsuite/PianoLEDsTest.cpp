#include "gtest/gtest.h"
#include "ArduinoTestFixture.h"

#include "ArduinoMock.h"
#include "PianoLEDs.h"
#include "LEDStripMock.h"

using namespace ::testing;

class PianoLEDsTest : public ArduinoTestFixture
{
public:
    PianoLEDsTest():
        _pianoLEDs(_ledStripMock)
    {};

protected:
    StrictMock<LEDStripMock> _ledStripMock;
    PianoLEDs _pianoLEDs;
};

std::unique_ptr<::testing::NiceMock<ArduinoMock>> ArduinoTestFixture::_arduinoMock;

TEST_F(PianoLEDsTest, begin)
{
    EXPECT_CALL(_ledStripMock, show());
    EXPECT_CALL(_ledStripMock, setBrightness(10));

    _pianoLEDs.begin();
}

TEST_F(PianoLEDsTest, loop_no_fade)
{
    EXPECT_CALL(*_arduinoMock, millis())
        .WillRepeatedly(::testing::Return(5));
    _pianoLEDs.loop();
}

TEST_F(PianoLEDsTest, loop_fade_to_blue)
{
    EXPECT_CALL(_ledStripMock, numPixels())
        .WillRepeatedly(::testing::Return(2));

    // loop 1
    EXPECT_CALL(*_arduinoMock, millis())
        .WillOnce(::testing::Return(5))  // isElapsed()
        .WillOnce(::testing::Return(5)); // lap()
    EXPECT_CALL(_ledStripMock, setPixelColor(0, 1, 1, 1));
    EXPECT_CALL(_ledStripMock, setPixelColor(1, 1, 1, 1));
    EXPECT_CALL(_ledStripMock, show());

    _pianoLEDs.fadeToBlue();
    _pianoLEDs.loop();

    // loop 2
    EXPECT_CALL(*_arduinoMock, millis())
        .WillOnce(::testing::Return(10))  // isElapsed()
        .WillOnce(::testing::Return(10)); // lap()
    EXPECT_CALL(_ledStripMock, setPixelColor(0, 2, 2, 2));
    EXPECT_CALL(_ledStripMock, setPixelColor(1, 2, 2, 2));
    EXPECT_CALL(_ledStripMock, show());

    _pianoLEDs.loop();

    // loop 3
    EXPECT_CALL(*_arduinoMock, millis())
        .WillOnce(::testing::Return(15))  // isElapsed()
        .WillOnce(::testing::Return(15)); // lap()
    EXPECT_CALL(_ledStripMock, setPixelColor(0, 3, 3, 3));
    EXPECT_CALL(_ledStripMock, setPixelColor(1, 3, 3, 3));
    EXPECT_CALL(_ledStripMock, show());

    _pianoLEDs.loop();
    //...

}
