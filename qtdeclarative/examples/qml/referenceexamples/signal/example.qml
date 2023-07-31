// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import People
import QtQuick  // For QColor

BirthdayParty {
// ![0]
    onPartyStarted: (time) => { console.log("This party started rockin' at " + time); }
// ![0]

    host: Boy {
        name: "Bob Jones"
        shoe { size: 12; color: "white"; brand: "Nike"; price: 90.0 }
    }

    Boy {
        name: "Leo Hodges"
        BirthdayParty.rsvp: "2009-07-06"
        shoe { size: 10; color: "black"; brand: "Reebok"; price: 59.95 }
    }
    Boy {
        name: "Jack Smith"
        shoe { size: 8; color: "blue"; brand: "Puma"; price: 19.95 }
    }
    Girl {
        name: "Anne Brown"
        BirthdayParty.rsvp: "2009-07-01"
        shoe.size: 7
        shoe.color: "red"
        shoe.brand: "Marc Jacobs"
        shoe.price: 699.99
    }
// ![1]
}
// ![1]
