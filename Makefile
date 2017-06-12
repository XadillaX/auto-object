MOCHA = ./node_modules/.bin/_mocha
ISTANBUL = ./node_modules/.bin/istanbul
COVERALLS = ./node_modules/coveralls/bin/coveralls.js

clean:
	@rm -rf node_modules

test:
	@$(MOCHA)

test-coveralls:
	@$(ISTANBUL) cover $(MOCHA) \
		--report lcovonly \
		-- \
		-R spec && cat ./coverage/lcov.info | \
		\
		$(COVERALLS) && rm -rf ./coverage 

.PHONY: test
