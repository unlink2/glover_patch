/**
 * Draws a memory layout given as an object as a html table
 * {
 *  hbits: 32, // horizontal bits
 *  increments: 8,
 *  layout: [
 *      {
 *          bits: 8,
 *          name: 'Name',
 *          type: 'typename'
 *      }
 *  ],
 *  types: {
 *      'typename': {color: '#color'}
 *  }
 * }
 */

let drawMemoryLayout = (layout, targetId, tableClass='memoryTable', tableId=null) => {
    const defaultIncrements = 8;
    const defaultHBits = 32;
    const defaultTypes = {
        'char': {color: 'lightblue'},
        'bitfield': {color: 'lightgreen'},
        'char[]': {color: 'lightblue'},
        'int32': {color: 'yellow'},
        'unknown': {color: 'red'}
    }

    if (!layout.hbits) {
        layout.hbits = defaultHBits;
    }
    if (!layout.increments) {
        layout.increments = defaultIncrements;
    }
    if (!layout.types) {
        layout.types = defaultTypes;
    }

    let target = document.getElementById(targetId);

    let table = document.createElement('table');

    if (tableClass) {
        table.classList.add(tableClass);
    }

    if (tableId) {
        table.id = tableId;
    }

    // draw table head
    for (let i = 0; i < 32; i+=layout.increments) {
        let header = document.createElement('th');
        header.innerText = i;
        table.appendChild(header);
    }



    target.appendChild(table);
}
