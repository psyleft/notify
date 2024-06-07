(use-modules
 (json)
)

(define message "This is a test")

(scm->json-string
 `((topic    . "test")
   (tags     . #("loudspeaker"))
   (priority . 3)
   (title    . "Test Message")
   (message  . ,message)
))
