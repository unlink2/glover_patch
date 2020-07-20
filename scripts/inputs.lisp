; enables/disables input polling
(defun 'toggleinputs '() '(pokei8 0x802AE8FC (^ (peeki8 0x802AE8FC) 0x4)))

; toggles an a press
(defun 'pressa '() '(pokei8 0x801FB280 (^ 0x80 (peeki8 0x801FB280))))


